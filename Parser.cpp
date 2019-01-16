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

// These are keywords that can appear at the beginning of a statement
const static std::set<string> statements =
{
    "do", "while", "for", "if", "case", "else", "enum"
};

const static std::set<string> storage_classes = {
    "static", "typedef", "extern", "auto", "register", "_Thread_local"
};

const static std::set<string> type_specifiers = {
    "void", "char", "short", "int", "long", "float", "double",
    "signed", "unsigned", "_Bool", "_Complex"
};

const static std::set<string> op_unary = {
    "++", "--", "!", "sizeof", "_Alignof", "~"
};

const static std::unordered_map<string, string> op_pairs = {
    {"(", ")"},
    {"[", "]"}
};

TreeNode* Parser::ParseDeclaration(){
    
}

TreeNode* Parser::ParseOperator(){
    Token tok = *stackPos--;
    string attr = tok.prettyPrint();
    if(attr == ")" || attr == "]" || attr == ";"){
        // error: unexpected operator
    }
    TreeNode *node = new TreeNode(tok);
    // Both unary and binary ops add lhs.
    node->addChild(ParseFromStack());
    
    // !! Does not handle prefix unary operators.
    // Binary ops only add rhs.
    if(op_unary.find(attr) == op_unary.end()) {
        node->addChild(ParseFromStack());
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
    return ParseFromStack();
}

void Parser::Parse(){
    exprStack.clear();
    while(lexer.hasNext()){
        Token cur = lexer.getNext();
        // End of statement
        if(cur.type == Token::Tok_Punctuator
           && cur.prettyPrint() == ";"){
            root.addChild(ParseExpression());
            exprStack.clear();
        }
        else exprStack.push_back(cur);
    }
}

void Parser::prettyPrint(){
    std::cout << root.prettyPrint() << "\n";
}

Parser::Parser(const std::string& file){
    lexer.loadFile(file);
    Parse();
}

Parser::~Parser(){
    
}
