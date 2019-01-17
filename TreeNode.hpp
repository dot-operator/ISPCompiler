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

#include "Token.hpp"

class TreeNode {
private:
    std::vector<std::unique_ptr<TreeNode>> children;
    Token token;
    
public:
    void addChild(TreeNode* pChild);
    virtual std::string prettyPrint(unsigned tabDepth = 0);
    
    TreeNode(Token tok, bool prefOp = false);
    
    TreeNode(const TreeNode &node) = delete;
    TreeNode(TreeNode&& node) = default;
    TreeNode(){}
    ~TreeNode(){};
    
    bool prefixop = false;
};

#endif /* TreeNode_hpp */
