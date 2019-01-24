//
//  Token.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef Token_hpp
#define Token_hpp

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
    
    const std::string print(){
        string result;
        switch(type){
            default:
            case Int:
                return std::to_string(std::get<int>(attribute));
            case Float:
                return std::to_string(std::get<float>(attribute));
            case Char:
                result = std::get<char>(attribute) ;
                return "'" + result + "'";
        }
    };
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
    
    variant<string, Constant> attribute;
    
    unsigned posLine = 0, posColumn = 0, posSource = 0;
    
    const std::string prettyPrint(){
        std::string output;
        switch(type){
            default:
            case Tok_EOF:
                return "EOF";
            case Tok_Keyword:
            case Tok_Punctuator:
            case Tok_Identifier:
                return std::get<std::string>(attribute);
            case Tok_String:
                return "constant \"" + std::get<std::string>(attribute) + "\"";
            case Tok_Constant:
                return "constant " + std::get<Constant>(attribute).print();
        }
    };
    
    explicit operator std::string() {
        switch(type){
            default:
            case Tok_EOF:
                return "";
            case Tok_Keyword:
            case Tok_Punctuator:
            case Tok_Identifier:
                return std::get<std::string>(attribute);
            case Tok_String:
                return "\"" + std::get<std::string>(attribute) + "\"";
            case Tok_Constant:
                return std::get<Constant>(attribute).print();
        }
    };
    
    Token(){};
    ~Token(){};
    
    Token(tokType typ, string at){
        type = typ;
        attribute = at;
    }
};

#endif /* Token_hpp */
