//
//  LexicalAnalyzer.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/9/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "LexicalAnalyzer.hpp"
#include <iostream>
#include <fstream>
#include <set>

const std::set<string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "inline", "int", "long", "register", "restrict", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic",
    "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
};

const std::set<string> punctuators = {
    "[", "]", "(", ")", "{", "}", ".", "->",
    "++", "--", "&", "*", "+", "-", "~", "!",
    "/", "%", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "^", "|", "&&", "||",
    "?", ":", ";", "...",
    "=", "*=", "+=", "-=", "/=", "%=", "<<=", ">>=", "^=", "&=", "|=",
    ",", "#", "##",
    "<:", ":>", "<%", "%>", "%:", "%:%:"
};

void LexicalAnalyzer::loadFile(const string& filePath) {
    std::ifstream file(filePath);
    if(!file){
        std::cout << "Unable to open " << filePath << "\n";
        exit(-1);
    }
    
    string line;
    while(std::getline(file, line)){
        // Strip single-line comments before starting anything else
        if(line.find("//") != line.npos){
            line = line.substr(0, line.find("//"));
        }
        source += line + "\n";
    }
    file.close();
    sourcePos = source.begin();
}

char LexicalAnalyzer::nextChar(){
    if(sourcePos != source.end()){
        if((*sourcePos) == '\n'){
            ++curLine;
            curColumn = 0;
        }
        else ++curColumn;
        
        return *sourcePos++;
    }
    else return '\0';
}

char LexicalAnalyzer::curChar(){
    if(sourcePos != source.end())
        return *sourcePos;
    else return '\0';
}

bool LexicalAnalyzer::hasNext(){
    if(sourcePos != source.end())
        return true;
    return false;
}

Token LexicalAnalyzer::makeNumber(){
    string str;
    bool hadDecimal = false;
    for(char c = nextChar(); isdigit(c) || c == '.'; c=nextChar()){
        str += c;
        
        if(c == '.'){
            if(!hadDecimal){
                hadDecimal = true;
                continue;
            }
            else {
                error("Expected a digit in float constant.", getCurrent());
            }
        }
        
        // don't want to advance past a one-char token by accident
        if(!isdigit(curChar())){
            break;
        }
    }
    
    Token tok;
    Constant constant;
    if(hadDecimal){
        constant.type = Constant::Float;
        constant.attribute = std::stof(str);
    }
    else {
        constant.type = Constant::Int;
        constant.attribute = std::stoi(str);
    }
    tok.type = Token::Tok_Constant;
    tok.attribute = constant;
    return tok;
}

Token LexicalAnalyzer::makeChar(){
    char c = nextChar();
    if(c == '\'')
        error("Expected an alphanumeric character in char constant.", getCurrent());
    
    Token token;
    token.type = Token::Tok_Constant;
    token.attribute = c;
    
    if(nextChar() != '\'')
        error("Closing single quote expected at end of char constant.", getCurrent());
    return token;
}

Token LexicalAnalyzer::makeString(){
    string str;
    nextChar();
    for(char c = nextChar(); c != '"'; c = nextChar()){
        if(c == '\n'){
            error("String literal may not contain newline (use \n instead).", getCurrent());
        }
        str += c;
    }
    Token token;
    token.type = Token::Tok_String;
    token.attribute = str;
    return token;
}

bool LexicalAnalyzer::isIdentifierChar(char c){
    if(isalnum(c) || c == '_')
        return true;
    return false;
}

bool LexicalAnalyzer::isPunctuatorChar(char c){
    if(isIdentifierChar(c) || iswspace(c))
        return false;
    return true;
}

Token LexicalAnalyzer::makeIdentifier(){
    // Build attribute
    string str;
    for(char c = nextChar(); isIdentifierChar(c); c = nextChar()){
        str += c;
        
        if(!isIdentifierChar(curChar()))
            break;
    }
    
    Token tok;
    tok.attribute = str;
    // Check if it's a keyword
    if(keywords.find(str) != keywords.end()){
        tok.type = Token::Tok_Keyword;
        return tok;
    }
    tok.type = Token::Tok_Identifier;
    return tok;
}

Token LexicalAnalyzer::makePunctuator(){
    string str;
    for(char c = nextChar(); isPunctuatorChar(c); c = nextChar()){
        str += c;
        
        if(!isPunctuatorChar(curChar()))
            break;
    }
    
    Token token;
    token.attribute = str;
    token.type = Token::Tok_Punctuator;
    if(punctuators.find(str) != punctuators.end()){
        return token;
    }
    
    // We get here if nothing else worked.
    error("Not a recognized token.", getCurrent());
    exit(-1);
}

Token LexicalAnalyzer::getNext(){
    Token t;
    if(!hasNext())
        return Token();
    
    char c =curChar();
    while(iswspace(c)){
        nextChar();
        c = curChar();
    }
    
    if(c == '\0'){
        return Token();
    }
    
    // Numeric constant
    else if(isdigit(c)){
        t = makeNumber();
    }
    // String literal
    else if(c == '"'){
        t = makeString();
    }
    
    else if(c == ('\'')){
        t = makeChar();
    }
    
    // Try keywords and identifiers.
    else if(isalpha(c) || c == '_')
        t = makeIdentifier();
        
    // Finally, try to find a punctuator.
    else t = makePunctuator();
    
    t.posLine = curLine;
    t.posColumn = curColumn;
    current = t;
    return t;
}

void LexicalAnalyzer::error(const string &msg, Token token){
    std::cerr << "Error: " << msg << " at line ";
    std::cerr << token.posLine << ", " << token.posColumn;
    exit(-1);
}

LexicalAnalyzer::LexicalAnalyzer() {
    
}

LexicalAnalyzer::~LexicalAnalyzer(){
    
}
