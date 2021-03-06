//
//  FunctionTreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/17/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "FunctionTreeNode.hpp"

void FunctionTreeNode::setName(const string &n){
    name = n;
}

void FunctionTreeNode::setType(const string &t, const string& storage){
    type = t;
    storageclass = storage;
}

void FunctionTreeNode::setBody(TreeNode *compound_statement){
    body = std::unique_ptr<TreeNode>(compound_statement);
}

void FunctionTreeNode::addParameter(DeclarationTreeNode *parameter){
    parameters.push_back(std::unique_ptr<DeclarationTreeNode>(parameter));
}

void FunctionTreeNode::setStorage(const string &storage){
    storageclass = storage;
}

void FunctionTreeNode::generateIR(){
    if(irGenerated)
        return;
    irGenerated = true;
    
    irOutput = "L_" + name + "\n";
    irOutput += body->getIRCode() + "\n";
    
    // insert a return at the end of the function if it's void
    // just in case
    if(type.find("void") != type.npos){
        irOutput += "return\n\n";
    }
}

const string FunctionTreeNode::prettyPrint(unsigned tabDepth){
    string output;
    output += makeTabs(tabDepth);
    
    output += storageclass + " " + type + " " + name + ":\n";
    if(!parameters.empty()){
        output += makeTabs(tabDepth) + "Parameters: \n";
        for(auto& param : parameters){
            output += param->prettyPrint(tabDepth + 1);
        }
    }
    if(body){
        output += makeTabs(tabDepth) + "Body: \n";
        output += body->prettyPrint(tabDepth + 1);
    }
    
    //output += makeTabs(tabDepth);
    return output;
}

FunctionTreeNode::FunctionTreeNode(Token tok){
    token = tok;
}

