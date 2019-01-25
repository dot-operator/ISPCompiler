//
//  FunctionCallTreeNode.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/25/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef FunctionCallTreeNode_hpp
#define FunctionCallTreeNode_hpp

#include "TreeNode.hpp"
#include "SymbolTable.hpp"

class FunctionCallTreeNode : public TreeNode {
private:
    Symbol symbol;
    std::vector<std::unique_ptr<TreeNode>> parameters;
public:
    void addParameter(TreeNode* parameter);
    
    void generateIR() override;
    const string prettyPrint(unsigned tabDepth = 0) override;
    
    FunctionCallTreeNode(Token tok, const Symbol& sym);
    
    FunctionCallTreeNode(const FunctionCallTreeNode &node) = delete;
    FunctionCallTreeNode(FunctionCallTreeNode&& node) = default;
    ~FunctionCallTreeNode(){};
};

#endif /* FunctionCallTreeNode_hpp */
