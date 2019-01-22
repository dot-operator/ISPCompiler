//
//  LexicalAnalyzer.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/9/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef LexicalAnalyzer_hpp
#define LexicalAnalyzer_hpp

#include "Token.hpp"

class LexicalAnalyzer {
public:
    LexicalAnalyzer();
    ~LexicalAnalyzer();
    
    bool hasNext();
    Token getNext();
    Token getCurrent();
    
    void error(const string& msg, Token token);
    void loadFile(const string& filePath);
private:
    string source;
    string::iterator sourcePos;
    string::iterator lastPos;
    Token current;
    
    char curChar();
    char nextChar();
    bool isIdentifierChar(char c);
    bool isPunctuatorChar(char c);
    bool isPunctuatorValid(const string& s);
    
    inline bool isNumberChar(char c){
        if(c != '.' && !isdigit(c))
            return false;
        return true;
    }
    void backOne();
    
    unsigned curLine = 0, curColumn = 0;
    unsigned lastLine = 0, lastColumn = 0;
    
    Token makeChar();
    Token makeNumber();
    Token makeString();
    Token makeIdentifier();
    Token makePunctuator();
};

#endif /* LexicalAnalyzer_hpp */
