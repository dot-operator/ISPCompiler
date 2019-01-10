//
//  LexicalAnalyzer.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/9/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef LexicalAnalyzer_hpp
#define LexicalAnalyzer_hpp

#include <string>
#include <variant>
#include <vector>

using std::string;
using std::variant;

struct Constant {
    enum constType {
        Int,
        Float,
        // Const_Enum,
        Char
    } type;
    variant<int, float, char> attribute;
};

struct Token {
    enum tokType {
        Tok_EOF,
        Tok_Keyword,
        Tok_Identifier,
        Tok_Constant,
        Tok_String,
        Tok_Punctuator
    } type;
    
    variant<char, string, Constant> attribute;
    
    unsigned posLine = 0, posColumn = 0;
};

class LexicalAnalyzer {
public:
    LexicalAnalyzer();
    ~LexicalAnalyzer();
    
    bool hasNext();
    Token getNext();
    
    void loadFile(const string& filePath);
private:
    string source;
    string::iterator sourcePos;
    
    char curChar();
    char nextChar();
    bool isIdentifierChar(char c);
    
    unsigned curLine = 0, curColumn = 0;
    
    Token makeNumber();
    Token makeString();
    Token makeIdentifier();
    Token makePunctuator();
};

#endif /* LexicalAnalyzer_hpp */
