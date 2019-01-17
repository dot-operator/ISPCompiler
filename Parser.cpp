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

typedef std::set<string> strings;

// These are keywords that can appear at the beginning of a statement
const static strings statements =
{
    "do", "while", "for", "if", "case", "else", "enum"
};

const static strings storage_classes = {
    "static", "typedef", "extern", "auto", "register", "_Thread_local"
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
    {"(", ")"},
    {"[", "]"}
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
        lexer.error(attr + " expected. Got " + cur.prettyPrint() + "instead.", cur);
    }
}

TreeNode* Parser::ParseOperator(){
    Token tok = *stackPos--;
    string attr = tok.prettyPrint();
    if(attr == ")" || attr == "]" || attr == ";"){
        // error: unexpected operator
        lexer.error("Unexpected punctuator in expression.", tok);
    }
    TreeNode *node = new TreeNode(tok);
    // Both unary and binary ops add lhs.
    node->addChild(ParseFromStack());
    
    // !! Does not handle prefix unary operators.
    // Binary ops only add rhs.
    if(op_unary.find(attr) == op_unary.end()) {
        node->addChild(ParseFromStack());
    }
    else {
        node->prefixop = (tok.posSource < (*stackPos).posSource);
    }
    return node;
}

TreeNode* Parser::ParseIdentifier() {
    TreeNode *node = new TreeNode(*stackPos);
    decStackPos();
    // !! Handle symbol table here.
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
    
    // Shunting yard!
    for(auto& tok : exprStack){
        string val = tok.prettyPrint();
        string opval;
        
        switch(tok.type){
            // Identifiers and constants go straight to output
            default:
            case Token::Tok_String:
            case Token::Tok_Constant:
            case Token::Tok_Identifier:
                outputStack.push_back(tok);
                break;
                
            // Assume these are all operators
            case Token::Tok_Punctuator:
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
                      (op_precedence.at(opStack.back().prettyPrint()).precedence >= op_precedence.at(tok.prettyPrint()).precedence) ){
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

TreeNode* Parser::ParseExpressionTokens(bool inSelection){
    exprStack.clear();
    while(lexer.hasNext()){
        Token cur = lexer.getNext();
        
        if(cur.prettyPrint() == "("){
            ++numOpenParens;
        }
        else if(cur.prettyPrint() == ")" ){
            ++numCloseParens;
        }
        
        // End of statement checks
        if(cur.type == Token::Tok_Punctuator){
            if((inSelection && numCloseParens > numOpenParens)
            || (!inSelection && cur.prettyPrint() == ";")){
                return ParseExpression();
            }
        }
        exprStack.push_back(cur);
    }
    // ERROR: ';' expected.
    string err = inSelection ? "')' expected." : "';' expected.";
    lexer.error(err, lexer.getCurrent());
    return nullptr;
}

TreeNode* Parser::ParseStatement(){
    Token tok = lexer.getNext();
    string attr = tok.prettyPrint();
    switch(tok.type){
        case Token::Tok_Keyword:
            // ifs, dos, whiles, etc
            if(keyword_statements.find(attr) != keyword_statements.end()){
                return keyword_statements[attr]();
            }
            else return ParseDeclaration();
            break;
        case Token::Tok_Punctuator:
            if(tok.prettyPrint() == "{"){
                return ParseCompoundStatement();
            }
        default:
            // ExpressionStatement
            return ParseExpression();
    }
}

TreeNode* Parser::ParseCompoundStatement(){
    symboltable.incScope();
    
    TreeNode* node = new TreeNode(lexer.getCurrent());
    for(Token tok = lexer.getNext(); tok.prettyPrint() != "}"; tok = lexer.getNext()){
        node->addChild(ParseStatement());
    }
    
    symboltable.decScope();
    return node;
}

TreeNode* Parser::ParseIf(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, "(");
    
    node->addChild(ParseExpressionTokens(true));
    // Expression parsing takes care of closing paren.
    
    node->addChild(ParseStatement());
    return node;
}

TreeNode* Parser::ParseWhile(){
    TreeNode* node = new TreeNode(lexer.getCurrent());
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, "(");
    
    node->addChild(ParseExpressionTokens(true));
    // Expression parsing takes care of closing paren.
    
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
    node->addChild(ParseExpressionTokens(true));
    // Expression parsing takes care of closing paren.
    
    lexer.getNext();
    expect(Token::Tok_Punctuator, ";");
    
    return node;
}

TreeNode* Parser::ParseDeclaration(){
    
}

void Parser::Parse(){
    while(lexer.hasNext()){
        root.addChild(ParseStatement());
    }
}

void Parser::prettyPrint(){
    std::cout << root.prettyPrint() << "\n";
}

Parser::Parser(const std::string& file){
    std::function<TreeNode*()> f = std::bind(&Parser::ParseIf, this);
    lexer.loadFile(file);
    Parse();
}

Parser::~Parser(){
    
}
