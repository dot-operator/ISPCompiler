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
#include "SymbolTable.hpp"
#include <functional>

using std::vector;

class Parser {
private:
    enum ExpressionTerminator {
        ExprEnd_Semicolon,
        ExprEnd_Paren,
        ExprEnd_Comma
    };
    
    SymbolTable symboltable;
    LexicalAnalyzer lexer;
    TreeNode root;
    
    void expect(Token tok);
    void expect(Token::tokType type, string attr = "");
    
    void Parse();
    TreeNode* ParseCompoundStatement();
    TreeNode* ParseDeclaration(bool isParameter = false);
    
    // Non-expression or -declaration statements
    TreeNode* ParseIf();
    TreeNode* ParseDo();
    TreeNode* ParseWhile();
    TreeNode* ParseFor();

#define FUNC(x) std::bind(&Parser::Parse##x, this)
    std::unordered_map<string, std::function<TreeNode*()>> keyword_statements = {
        {"if", FUNC(If)},
        {"do", FUNC(Do)},
        {"while", FUNC(While)},
        {"for", FUNC(For)},
    };
#undef FUNC
    
    // Jump statements
    TreeNode* ParseReturn();
    
    // These return a pointer to memory;
    // probably want to put them directly
    // in a unique_ptr
    // Declared in order of descending call order
    TreeNode* ParseExpressionTokens(ExpressionTerminator end = ExprEnd_Semicolon, bool* lastParam = nullptr);
    TreeNode* ParseExpression();
    TreeNode* ParseFromStack();
    TreeNode* ParseOperator();
    TreeNode* ParseIdentifier();
    TreeNode* ParseFunctionCall();
    TreeNode* ParseConstant();
    // Operator Precedence Parsing (Shunting Yard)
    vector<Token> exprStack, opStack, outputStack;
    vector<Token>::iterator stackPos;
    unsigned numOpenParens = 0, numCloseParens = 0;
    
    
    inline void decStackPos(){
        // originally had atBeginning - what was I using it for?
        if(stackPos != outputStack.begin())
            --stackPos;
    }
    
    // Top level parse.
    // Adds statements to document root node.
    TreeNode* ParseStatement();
public:
    void prettyPrint();
    void makeIR(const string& file);
    
    Parser(const std::string& file);
    ~Parser();
};

#endif /* Parser_hpp */
