//
//  TreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "TreeNode.hpp"

string TreeNode::generateIR(){
    if(!irOutput.empty())
        return irOutput;
    
    
    return irOutput;
}

void TreeNode::addChild(TreeNode *pChild){
    children.push_back(std::unique_ptr<TreeNode>(pChild));
}

string TreeNode::prettyPrint(unsigned tabDepth){
    string output;
    output += makeTabs(tabDepth);
    
    output += token.prettyPrint() + ":\n";
    for(auto& child : children){
        output += child->prettyPrint(tabDepth + 1);
    }
    
    //output += makeTabs(tabDepth);
    return output;
}

TreeNode::TreeNode(Token tok, bool prefOp){
    token = tok;
    prefixop = prefOp;
}
