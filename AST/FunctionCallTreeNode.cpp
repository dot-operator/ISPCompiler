//
//  FunctionCallTreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/25/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "FunctionCallTreeNode.hpp"

void FunctionCallTreeNode::addParameter(TreeNode *parameter){
    parameters.push_back(std::unique_ptr<TreeNode>(parameter));
}

void FunctionCallTreeNode::generateIR(){
    if(irGenerated)
        return;
    irGenerated = true;
    
    string paramIR;
    for(auto& node : parameters){
        // Build up IR for parameter expressions...
        paramIR += node->getIRCode() + "\n";
        // ... and also build the function call itself.
        irOutput += "param t";
        irOutput += to_string(node->getIRName()) + "\n";
    }
    // concatenate
    irOutput = paramIR + "\n" + irOutput + "t" + to_string(getIRName()) + " = ";
    // actual function call
    irOutput += "call L_" + symbol.name + ", " + to_string(parameters.size());
    irOutput += "\n";
}

const string FunctionCallTreeNode::prettyPrint(unsigned tabDepth){
    string output;
    output += makeTabs(tabDepth);
    
    auto& [name, type, depth, func, numParams, numUses] = symbol;
    output += "Call to " + name + " with parameters:\n";
    
    if(!parameters.empty()){
        for(auto& param : parameters){
            output += param->prettyPrint(tabDepth + 1);
        }
    }
    
    //output += makeTabs(tabDepth);
    return output;
}

FunctionCallTreeNode::FunctionCallTreeNode(const Symbol& sym){
    symbol = sym;
}
