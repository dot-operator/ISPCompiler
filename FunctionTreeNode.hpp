//
//  FunctionTreeNode.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/17/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef FunctionTreeNode_hpp
#define FunctionTreeNode_hpp

#include "DeclarationTreeNode.hpp"

class FunctionTreeNode : public TreeNode {
private:
    string storageclass, type, name;
    unsigned pointers;
    std::unique_ptr<TreeNode> body;
    std::vector<std::unique_ptr<DeclarationTreeNode>> parameters;
public:
    void setBody(TreeNode* compound_statement);
    void setType(const string& type, const string& storage = "");
    void setName(const string& name);
    void setStorage(const string& storage);
    void addParameter(DeclarationTreeNode* parameter);
    
    void generateIR() override;
    const string prettyPrint(unsigned tabDepth = 0) override;
    
    FunctionTreeNode(Token tok);
    
    FunctionTreeNode(const FunctionTreeNode &node) = delete;
    FunctionTreeNode(FunctionTreeNode&& node) = default;
    ~FunctionTreeNode(){};
};

#endif /* FunctionTreeNode_hpp */
