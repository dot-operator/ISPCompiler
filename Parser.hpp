//
//  Parser.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include "LexicalAnalyzer.hpp"
#include "TreeNode.hpp"
#include "Precedence.hpp"

using std::vector;

class Parser {
private:
    LexicalAnalyzer lexer;
    TreeNode root;
    
    void Parse();
    
    TreeNode* ParseCompoundStatement();
    TreeNode* ParseDeclaration();
    
    // These return a pointer to memory;
    // probably want to put them directly
    // in a unique_ptr
    // Declared in order of descending call order
    TreeNode* ParseExpression();
    TreeNode* ParseFromStack();
    TreeNode* ParseOperator();
    TreeNode* ParseIdentifier();
    // kind of useless for now; can merge w/ identifiers
    TreeNode* ParseConstant();
    
    // Operator Precedence Parsing (Shunting Yard)
    vector<Token> exprStack, opStack, outputStack;
    vector<Token>::iterator stackPos;
    
    inline void decStackPos(){
        // originally had atBeginning - what was I using it for?
        if(stackPos != outputStack.begin())
            --stackPos;
    }
public:
    void prettyPrint();
    
    Parser(const std::string& file);
    ~Parser();
};

#endif /* Parser_hpp */
