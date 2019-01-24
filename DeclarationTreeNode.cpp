//
//  DeclarationTreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/17/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "DeclarationTreeNode.hpp"

void DeclarationTreeNode::setInitializer(TreeNode *expression){
    initializer = std::unique_ptr<TreeNode>(expression);
}

void DeclarationTreeNode::setType(const string &t, const string& storage){
    storageclass = storage;
    type = t;
}

void DeclarationTreeNode::setName(const string &n){
    name = n;
}

void DeclarationTreeNode::setStorage(const string &storage){
    storageclass = storage;
}

void DeclarationTreeNode::setTypeQualifier(const string &q){
    typequalifier = q;
}

void DeclarationTreeNode::generateIR(){
    if(initializer && !irGenerated){
        irOutput = initializer->getIRCode() + "\n";
        irOutput += name + " = t" + std::to_string(initializer->getIRName()) + "\n";
    }
    irGenerated = true;
}

const string DeclarationTreeNode::prettyPrint(unsigned tabDepth){
    string output;
    output += makeTabs(tabDepth);
    
    output += typequalifier + " " + storageclass + " " + type + " " + name + ":\n";
    if(initializer){
        output += initializer->prettyPrint(tabDepth + 1);
    }
    
    //output += makeTabs(tabDepth);
    return output;
}

DeclarationTreeNode::DeclarationTreeNode(Token tok){
    token = tok;
}
