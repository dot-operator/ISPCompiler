//
//  Parser.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "Parser.hpp"
#include <set>
#include <iostream>
#include <fstream>

#include "FunctionTreeNode.hpp"
#include "FunctionCallTreeNode.hpp"

typedef std::set<string> strings;

// These are keywords that can appear at the beginning of a statement
const static strings statements =
{
    "do", "while", "for", "if", "case", "else", "enum"
};

const static strings storage_classes = {
    "static", "typedef", "extern", "auto", "register", "_Thread_local"
};

const static strings type_qualifiers = {
    "const", "restrict", "volatile", "_Atomic"
};

const static strings jump_statements = {
    "goto", "continue", "break", "return"
};

const static strings type_specifiers = {
    "void", "char", "short", "int", "long", "float", "double",
    "signed", "unsigned", "_Bool", "_Complex"
};

const static strings op_unary = {
    "++", "--", "!", "sizeof", "_Alignof", "~"
};

const static std::unordered_map<string, string> op_pairs = {
    {")", "("},
    {"]", "["}
};

void Parser::expect(Token tok){
    expect(tok.type, tok.prettyPrint());
}

void Parser::expect(Token::tokType type, string attr){
    const static std::unordered_map<Token::tokType, string> types = {
        {Token::Tok_Keyword, "Keyword"},
        {Token::Tok_Punctuator, "Punctuator"},
        {Token::Tok_String, "String Literal"},
        {Token::Tok_Constant, "Number Constant"},
        {Token::Tok_Identifier, "Identifier"},
        {Token::Tok_EOF, "[EOF]"}
    };
    
    Token cur = lexer.getCurrent();
    if(cur.type != type){
        // Error.
        string error = types.at(type) + " expected. Got " + types.at(cur.type) + " instead.";
        lexer.error(error, cur);
    }
    else if(attr != "" && attr != cur.prettyPrint()){
        // Error.
        lexer.error(attr + " expected. Got " + cur.prettyPrint() + " instead.", cur);
    }
}

TreeNode* Parser::ParseOperator(){
    Token tok = *(stackPos--);
    string attr = tok.prettyPrint();
    if(attr == ")" || attr == "]" || attr == ";"){
        // error: unexpected operator
        lexer.error("Unexpected punctuator in expression.", tok);
    }
    TreeNode *node = new TreeNode(tok);
    // Both unary and binary ops add lhs.
    node->addChild(ParseFromStack());
    
    // Binary ops add rhs.
    if(op_unary.find(attr) == op_unary.end()) {
        node->addChild(ParseFromStack());
    }
    else {
        node->prefixop = (tok.posSource < (*stackPos).posSource);
    }
    return node;
}

TreeNode* Parser::ParseFunctionCall(){
    Symbol sym = *symboltable.find(stackPos->prettyPrint());
    decStackPos();
    
    FunctionCallTreeNode* fcallNode = new FunctionCallTreeNode(sym);
    for(unsigned i = 0; i < sym.parameters; ++i){
        fcallNode->addParameter(ParseFromStack());
    }
    
    return fcallNode;
}

TreeNode* Parser::ParseIdentifier() {
    if(Symbol *sym = symboltable.find(stackPos->prettyPrint()); sym){
        if(sym->function)
            return ParseFunctionCall();
    }
    else lexer.error("Undeclared identifier ", *stackPos);
    
    TreeNode *node = new TreeNode(*stackPos);
    decStackPos();
    return node;
}

TreeNode* Parser::ParseConstant() {
    TreeNode* node = new TreeNode(*stackPos);
    decStackPos();
    return node;
}

TreeNode* Parser::ParseFromStack() {
    if(stackPos < outputStack.begin()){
        // error
        lexer.error("Internal error parsing operator precedence!!", lexer.getCurrent());
    }
    
    switch(stackPos->type){
        default:
            // Error: Unexpected type at expression root.
            return nullptr;
        case Token::Tok_Punctuator:
            return ParseOperator();
        case Token::Tok_String:
        case Token::Tok_Constant:
            return ParseConstant();
        case Token::Tok_Identifier:
            return ParseIdentifier();
    }
}

TreeNode* Parser::ParseExpression() {
    outputStack.clear();
    
    // Shunting yard
    for(auto& tok : exprStack){
        string val = tok.prettyPrint();
        string opval;
        
        Symbol *sym = nullptr;
        switch(tok.type){
            // Identifiers and constants go straight to output
            case Token::Tok_Identifier:
                sym = symboltable.find(tok.prettyPrint());
                if(!sym)
                    lexer.error("Undeclared identifier: " + tok.prettyPrint(), tok);
                if(sym->function){
                    opStack.push_back(tok);
                    continue;
                }
                
            default:
            case Token::Tok_String:
            case Token::Tok_Constant:
                outputStack.push_back(tok);
                continue;
                
            // Assume these are all operators
            case Token::Tok_Punctuator:
                if(val == ",")
                    continue;
                // Paren/bracket opening
                if(opStack.empty() || val == "(" || val == "["){
                    opStack.push_back(tok);
                    continue;
                }
                // If we have paren or brackets closing,
                // unload everything on the stack
                // up to the matching opening
                else if(!opStack.empty()
                        && op_pairs.find(val) != op_pairs.end()) {
                    opval = opStack.back().prettyPrint();
                    while(!opStack.empty() && opval != op_pairs.at(val)){
                        outputStack.push_back(opStack.back());
                        opStack.pop_back();
                        opval = opStack.back().prettyPrint();
                    }
                    // Pop opening paren/bracket
                    opStack.pop_back();
                    continue;
                }
                // Higher-precedence ops get to output first
                while(!opStack.empty() &&
                      ((opStack.back().type == Token::Tok_Identifier) || (op_precedence.at(opStack.back().prettyPrint()).precedence >= op_precedence.at(tok.prettyPrint()).precedence)) ){
                    outputStack.push_back(opStack.back());
                    opStack.pop_back();
                }
                // And then we get to go on the operator stack
                opStack.push_back(tok);
                break;
        }
    }
    
    // Move any leftover operators to the output.
    while(!opStack.empty()){
        outputStack.push_back(opStack.back());
        opStack.pop_back();
    }
    // Unload the stack into a tree.
    stackPos = outputStack.end() - 1;
    exprStack.clear();
    return ParseFromStack();
}

TreeNode* Parser::ParseExpressionTokens(ExpressionTerminator end, bool* lastParam){
    string op;
    switch(end){
        default:
        case ExprEnd_Semicolon:
            op = ";";
            break;
        case ExprEnd_Comma:
            op = ",";
            break;
        case ExprEnd_Paren:
            op = ")";
            break;
    }
    
    exprStack.clear();
    outputStack.clear();
    numOpenParens = numCloseParens = 0;
    for(Token cur = lexer.getCurrent(); lexer.hasNext(); cur = lexer.getNext()){
        string curOp = cur.prettyPrint();
        if(curOp == "("){
            ++numOpenParens;
        }
        else if(curOp == ")" ){
            ++numCloseParens;
        }
        
        // End of statement checks
        if(cur.type == Token::Tok_Punctuator){
            if(curOp == op) return ParseExpression();
            
            if(numCloseParens > numOpenParens &&
               (end == ExprEnd_Paren || end == ExprEnd_Comma)){
                if(lastParam){
                    *lastParam = true;
                }
                return ParseExpression();
            }
        }
        exprStack.push_back(cur);
    }
    // ERROR: ';' expected.
    string err = op + " expected.";
    lexer.error(err, lexer.getCurrent());
    return nullptr;
}

TreeNode* Parser::ParseStatement(){
    Token tok = lexer.getCurrent();
    string attr = tok.prettyPrint();
    switch(tok.type){
        case Token::Tok_Keyword:
            // ifs, dos, whiles, etc
            if(keyword_statements.find(attr) != keyword_statements.end()){
                return keyword_statements[attr]();
            }
            else if(jump_statements.find(attr) != jump_statements.end()){
                return ParseReturn();
            }
            else return ParseDeclaration();
            break;
        case Token::Tok_Punctuator:
            if(tok.prettyPrint() == "{"){
                return ParseCompoundStatement();
            }
        default:
            // ExpressionStatement
            return ParseExpressionTokens();
    }
}

TreeNode* Parser::ParseCompoundStatement(){
    symboltable.incScope();
    
    TreeNode* node = new TreeNode(lexer.getCurrent());
    for(Token tok = lexer.getNext(); tok.prettyPrint() != "}";tok = lexer.getNext()){
        node->addChild(ParseStatement());
    }
    
    // Generate IR before all of the relevant symbols go out of scope.
    node->getIRCode();
    
    symboltable.decScope();
    return node;
}

TreeNode* Parser::ParseIf(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, "(");
    lexer.getNext();
    node->addChild(ParseExpressionTokens(ExprEnd_Paren));
    // Expression parsing takes care of closing paren.
    
    lexer.getNext();
    node->addChild(ParseStatement());
    return node;
}

TreeNode* Parser::ParseFor(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getCurrent();
    expect(Token::Tok_Punctuator, "(");
    node->addChild(ParseDeclaration());
    lexer.getNext();
    node->addChild(ParseExpressionTokens()); // expect ;
    lexer.getNext();
    node->addChild(ParseExpressionTokens(ExprEnd_Paren)); // expect )
    
    lexer.getNext();
    node->addChild(ParseStatement());
    return node;
}

TreeNode* Parser::ParseWhile(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, "(");
    lexer.getNext();
    node->addChild(ParseExpressionTokens(ExprEnd_Paren));
    // Expression parsing takes care of closing paren.
    
    lexer.getNext();
    node->addChild(ParseStatement());
    return node;
}

TreeNode* Parser::ParseDo(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getNext();
    node->addChild(ParseStatement());
    
    lexer.getNext();
    expect(Token::Tok_Keyword, "while");
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, "(");
    lexer.getNext();
    node->addChild(ParseExpressionTokens(ExprEnd_Paren));
    // Expression parsing takes care of closing paren.
    lexer.getNext();
    expect(Token::Tok_Punctuator, ";");
    
    return node;
}

TreeNode* Parser::ParseReturn(){
    expect(Token::Tok_Keyword, "return");
    TreeNode* node = new TreeNode(lexer.getCurrent());
    lexer.getNext();
    node->addChild(ParseExpressionTokens());
    return node;
}

TreeNode* Parser::ParseDeclaration(bool isParameter){
    string storage, name, type, type_q;
    Token decStart = lexer.getCurrent();
    if(type_qualifiers.find(decStart.prettyPrint()) != type_qualifiers.end()){
        type_q = decStart.prettyPrint();
        lexer.getNext();
    }
    if(storage_classes.find(decStart.prettyPrint()) != storage_classes.end()){
        storage = decStart.prettyPrint();
        lexer.getNext();
    }
    string ctype = lexer.getCurrent().prettyPrint();
    if(type_specifiers.find(ctype) == type_specifiers.end())
        lexer.error("Expected a type. Got " + ctype, lexer.getCurrent());
    else type = ctype;
    
    lexer.getNext();
    expect(Token::Tok_Identifier);
    name = lexer.getCurrent().prettyPrint();
    auto tableentry = symboltable.find(name);
    if(tableentry){
        lexer.error("Redeclaration of symbol " + name, lexer.getCurrent());
    }
    
    Token op = lexer.getNext();
    expect(Token::Tok_Punctuator);
    string punc = op.prettyPrint();
    if(!isParameter && punc == "("){
        Symbol sym;
        sym.name = name;
        sym.type = type;
        sym.function = true;
        
        FunctionTreeNode* fnode = new FunctionTreeNode(decStart);
        fnode->setType(type);
        fnode->setName(name);
        fnode->setStorage(storage);
        lexer.getNext();
        string curTok = lexer.getCurrent().prettyPrint();
        while(curTok != ")"){
            fnode->addParameter(dynamic_cast<DeclarationTreeNode*>(ParseDeclaration(true)));
            ++sym.parameters;
            
            curTok = lexer.getCurrent().prettyPrint();
            if(curTok == ")")
                break;
            lexer.getNext();
        }
        symboltable.addSymbol(sym);
        
        lexer.getNext();
        expect(Token::Tok_Punctuator, "{");
        fnode->setBody(ParseCompoundStatement());
        return fnode;
    }
    
    if(isParameter){
        symboltable.addParameter(name, type);
    }
    else symboltable.addSymbol(name, type, false);
    DeclarationTreeNode* dnode = new DeclarationTreeNode(decStart);
    dnode->setName(name);
    dnode->setType(type);
    dnode->setStorage(storage);
    dnode->setTypeQualifier(type_q);
    if(punc == "="){
        lexer.getNext();
        dnode->setInitializer(ParseExpressionTokens());
        return dnode;
    }
    
    if(!isParameter)
        expect(Token::Tok_Punctuator, ";");
    return dnode;
}

void Parser::Parse(){
    while(lexer.hasNext()){
        lexer.getNext();
        root.addChild(ParseStatement());
    }
}

void Parser::prettyPrint(){
    std::cout << root.prettyPrint() << "\n";
}

void Parser::makeIR(const string& file){
    std::ofstream output;
    output.open(file);
    output << root.getIRCode() << "\n";
    output.close();
    std::cout << "Wrote IR to " << file << std::endl;
}

Parser::Parser(const std::string& file){
    //std::function<TreeNode*()> f = std::bind(&Parser::ParseIf, this);
    lexer.loadFile(file);
    Parse();
}

Parser::~Parser(){
    
}
