//
//  TreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "TreeNode.hpp"

void TreeNode::addChild(TreeNode *pChild){
    children.push_back(std::unique_ptr<TreeNode>(pChild));
}

std::string TreeNode::prettyPrint(unsigned tabDepth){
    std::string output;
    for(unsigned i = 0; i < tabDepth; ++i){
        output += '\t';
    }
    
    output += token.prettyPrint() + ":\n";
    for(auto& child : children){
        output += child->prettyPrint(tabDepth + 1);
    }
    
    for(unsigned i = 0; i < tabDepth; ++i){
        output += '\t';
    }
    return output;
}

TreeNode::TreeNode(Token tok){
    token = tok;
}
