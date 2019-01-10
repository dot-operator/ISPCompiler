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

const string keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "inline", "int", "long", "register", "restrict", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic",
    "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
};

const string punctuators[] = {
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
        source += line;
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
    for(char c = nextChar(); isdigit(c); c=nextChar()){
        str += c;
        
        if(!isdigit(c)){
            if(c == '.'){
                if(!hadDecimal){
                    hadDecimal = true;
                    continue;
                }
                else {
                    std::cout << "Lexer error: Expected a digit in float constant.\n";
                    exit(-1);
                }
            }
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

Token LexicalAnalyzer::makeString(){
    string str;
    nextChar();
    for(char c = nextChar(); c != '"'; c = nextChar()){
        if(c == '\n'){
            std::cout << "Lexer error: String literal may not contain newline (use \n instead).\n";
            exit(-1);
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
    for(auto& kwd : keywords){
        if(str == kwd){
            tok.type = Token::Tok_Keyword;
            return tok;
        }
    }
    tok.type = Token::Tok_Identifier;
    return tok;
}

Token LexicalAnalyzer::makePunctuator(){
    char t = nextChar();
    Token token;
    for(auto& p : punctuators){
        bool fail = false;
        for(auto& c : p){
            if(t != c){
                fail = true;
                break;
            }
        }
        
        if(!fail){
            token.type = Token::Tok_Punctuator;
            token.attribute = p;
            return token;
        }
    }
    
    // We get here if nothing else worked.
    std::cout << "Lexer error: Not a recognized token.\n";
    exit(-1);
}

Token LexicalAnalyzer::getNext(){
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
    if(isdigit(c)){
        return makeNumber();
    }
    // String literal
    if(c == '"'){
        return makeString();
    }
    
    // Try keywords and identifiers.
    if(isalpha(c) || c == '_')
        return makeIdentifier();
        
    // Finally, try to find a punctuator.
    return makePunctuator();
}

LexicalAnalyzer::LexicalAnalyzer() {
    
}

LexicalAnalyzer::~LexicalAnalyzer(){
    
}
