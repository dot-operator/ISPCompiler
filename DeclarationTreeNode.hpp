//
//  DeclarationTreeNode.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/17/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef DeclarationTreeNode_hpp
#define DeclarationTreeNode_hpp

#include "TreeNode.hpp"

class DeclarationTreeNode : public TreeNode {
private:
    string storageclass, typequalifier, type, name;
    unsigned pointers;
    std::unique_ptr<TreeNode> initializer;
    
public:
    void setInitializer(TreeNode* expression);
    void setType(const string& type, const string& storage = "");
    void setName(const string& name);
    void setStorage(const string& storage);
    void setTypeQualifier(const string& q);
    inline void incPointerDepth(){
        ++pointers;
    }
    
    std::string prettyPrint(unsigned tabDepth = 0) override;
    
    DeclarationTreeNode(Token tok);
    
    DeclarationTreeNode(const DeclarationTreeNode &node) = delete;
    DeclarationTreeNode(DeclarationTreeNode&& node) = default;
    DeclarationTreeNode(){}
    ~DeclarationTreeNode(){};
};

#endif /* DeclarationTreeNode_hpp */
