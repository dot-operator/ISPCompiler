//
//  TreeNode.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef TreeNode_hpp
#define TreeNode_hpp

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Token.hpp"

using std::to_string;

class TreeNode {
private:
    inline const void expectChildren(const unsigned expected){
        if(children.size() < expected){
            error("Parser didn't generate enough nodes for the statement.");
        }
        else if(children.size() > expected){
            error("Parser generated too many nodes for the statement.");
        }
    };
    
    // Three-address code
    // Maybe this should be done with polymorphism :(
    const string generateReturn();
    const string generateIf();
    //const string generateElse();
    //const string generateLabel();
    const string generateDo();
    const string generateWhile();
    const string generateFor();
#define FUNC(x) std::bind(&TreeNode::generate##x, this)
    std::unordered_map<string, std::function<const string()>> ir_funcs = {
        {"if", FUNC(If)},
        {"do", FUNC(Do)},
        {"while", FUNC(While)},
        {"for", FUNC(For)},
        {"return", FUNC(Return)}
    };
#undef FUNC
    std::vector<std::unique_ptr<TreeNode>> children;
    
protected:
    Token token;
    
    static const inline string makeTabs(unsigned num){
        string output;
        for(unsigned i = 0; i < num; ++i){
            output += '\t';
        }
        return output;
    };
    
    virtual void generateIR();
    const void error(const string& msg);
    
    bool irGenerated = false;
    string irOutput;
    unsigned irSymbolName;
    static unsigned irTotalSymbols;
    
public:
    void addChild(TreeNode* pChild);
    virtual const string prettyPrint(unsigned tabDepth = 0);
    
    // Public 3-address code functions
    inline string getIRCode(){
        if(irGenerated)
            return irOutput;
        generateIR();
        return irOutput;
    };
    inline unsigned getIRName(){
        if(irGenerated)
            return irSymbolName;
        generateIR();
        return irSymbolName;
    };
    
    TreeNode(Token tok, bool prefOp = false);
    
    TreeNode(const TreeNode &node) = delete;
    TreeNode(TreeNode&& node) = default;
    TreeNode(){}
    ~TreeNode(){};
    
    bool prefixop = false;
};

#endif /* TreeNode_hpp */
