//
//  Precedence.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/11/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef Precedence_hpp
#define Precedence_hpp

// Operator Precedences
struct Precedence {
    unsigned precedence = 0;
    bool lhs = true;
    Precedence(unsigned p, bool l){
        precedence = p;
        lhs = l;
    }
};

#define OP(i,b) const static Precedence p##i(i,b);
OP(15, true);
OP(14, false);
OP(13, true);
OP(12, true);
OP(11, true);
OP(10, true);
OP(9, true);
OP(8, true);
OP(7, true);
OP(6, true);
OP(5, true);
OP(4, true);
OP(3, false);
OP(2, false);
OP(1, true);


#include <unordered_map>
#include <string>
const static std::unordered_map<std::string, const Precedence&> op_precedence = {
    {"++", p15},
    {"--", p15},
    {"(", p15},
    {")", p15},
    {"[", p15},
    {"]", p15},
    {".", p15},
    {"->", p15},
    
    {"++", p14},
    {"--", p14},
    //{"+", 14},
    //{"-", 14},
    {"!", p14},
    {"~", p14},
    //{"*", p14},
    //{"&", p14},
    {"sizeof", p14},
    {"_Alignof", p14},
    
    {"*", p13},
    {"/", p13},
    {"%", p13},
    
    {"+", p12},
    {"-", p12},
    
    {"<<", p11},
    {">>", p11},
    
    {"<", p10},
    {">", p10},
    {"<=", p10},
    {">=", p10},
    
    {"==", p9},
    {"!=", p9},
    
    {"&", p8},
    {"^", p7},
    {"|", p6},
    {"&&", p5},
    {"||", p4},
    
    {"?", p3},
    {":", p3},
    
    // Assignment has the lowest precedence
    {"=", p2},
    {"+=", p2},
    {"-=", p2},
    {"*=", p2},
    {"/=", p2},
    {"%=", p2},
    {"<<=", p2},
    {">>=", p2},
    {"&=", p2},
    {"^=", p2},
    {"|=", p2},
    
    {",", p1}
};

#endif /* Precedence_hpp */
