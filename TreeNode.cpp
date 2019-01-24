//
//  TreeNode.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/10/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "TreeNode.hpp"
#include <iostream>

using std::to_string;

unsigned TreeNode::irTotalSymbols = 0;

const string TreeNode::generateReturn(){
    if(children.size() == 0)
        return "return";
    
    return children[0]->getIRCode() + "\nreturn t" + to_string(children[0]->getIRName());
}

const string TreeNode::generateIf(){
    if(children.size() < 2)
        error("Parser should have generated an AST node for if expression \
              and body, but not enough associated nodes were found.");
    
    // expr
    string output = children[0]->getIRCode() + "\n";
    // if
    output += "ifFalse t" + to_string(children[0]->getIRName());
    output += " goto L" + to_string(irSymbolName) + "\n";
    // then
    output += children[1]->getIRCode();
    // else
    if(children.size() > 3)
        output += children[2]->getIRCode();
    // jump label
    output += "L" + to_string(irSymbolName);
    return output;
}

const string TreeNode::generateDo(){
    expectChildren(2);
    
    string output = "L" + to_string(irSymbolName) + "\n" + children[0]->getIRCode();
    output += "if t" + to_string(children[1]->getIRName());
    output += " goto L" + to_string(irSymbolName);
    return output;
}

const string TreeNode::generateWhile(){
    expectChildren(2);
    
    // while (expr)
    string output = "ifFalse t" + to_string(children[0]->getIRName());
    output += " goto L" + to_string(irSymbolName) + "\n";
    // do {body}
    output += children[1]->getIRCode();
    // jump label
    output += "L" + to_string(irSymbolName);
    return output;
}

// Parser makes for nodes with 4 children: dec, test, inc, body
const string TreeNode::generateFor(){
    expectChildren(4);
    
    // declaration
    string output = children[0]->getIRCode();
    // Test jump destination
    output += "L" + to_string(children[1]->getIRName()) + "\n";
    // test
    output += children[1]->getIRCode();
    output += "ifFalse t" + to_string(children[1]->getIRName());
    output += " goto L" + to_string(irSymbolName);
    // body
    output += children[2]->getIRCode();
    // inc
    output += children[3]->getIRCode();
    // end label
    output += "L" + to_string(irSymbolName);
    
    return output;
}

void TreeNode::generateIR(){
    if(irGenerated)
        return;
    
    irGenerated = true;
    for(auto& child : children){
        child->generateIR();
    }
    irSymbolName = irTotalSymbols++;
    
    switch(token.type){
        // Root node has EOF/fake token type.
        // It should be empty and just add all the child IR.
        case Token::Tok_EOF:
            break;
            
        default:
        case Token::Tok_Identifier:
            // Test for functions
        case Token::Tok_String:
        case Token::Tok_Constant:
            irOutput = "t" + to_string(irSymbolName) + " = ";
            irOutput += (string)token;
            break;
            
        case Token::Tok_Punctuator:
            if(token.prettyPrint() == "{"){
                break;
            }
            irOutput = "t" + to_string(irSymbolName) + " = ";
            // Generate operator IR
            switch(children.size()){
                case 0:
                    error("Expected an operand for operator " + token.prettyPrint());
                case 1:
                    if(prefixop)
                        irOutput += token.prettyPrint()
                        + "t" + to_string(children[0]->getIRName());
                    else irOutput += "t" + to_string(children[0]->getIRName())
                        + token.prettyPrint();
                    break;
                case 2:
                    irOutput += "t" + to_string(children[0]->getIRName());
                    irOutput += " " + token.prettyPrint() + " t";
                    irOutput += to_string(children[1]->getIRName());
                    break;
                default:
                case 3:
                    error("Too many operands for one " + token.prettyPrint() + ".");
            }
            break;
            
        case Token::Tok_Keyword:
            // return, if, else, do, while, for
            if(ir_funcs.find(token.prettyPrint()) != ir_funcs.end()){
                irOutput = ir_funcs[token.prettyPrint()]() + "\n";
                return;
            }
            else error("Don't know how to make IR for keyword " + token.prettyPrint() + ".");
            break;
    }
    
    string result;
    for(auto& child : children){
        result += child->getIRCode() + "\n";
    }
    irOutput = result + irOutput;
}

const void TreeNode::error(const string &msg){
    std::cerr << "ERROR: " << msg << "\nOn line ";
    std::cerr << token.posLine << ", " << token.posColumn << "\n";
    exit(-1);
}

void TreeNode::addChild(TreeNode *pChild){
    children.push_back(std::unique_ptr<TreeNode>(pChild));
}

const string TreeNode::prettyPrint(unsigned tabDepth){
    string output;
    output += makeTabs(tabDepth);
    
    output += token.prettyPrint() + ":\n";
    for(auto& child : children){
        output += child->prettyPrint(tabDepth + 1);
    }
    return output;
}

TreeNode::TreeNode(Token tok, bool prefOp){
    token = tok;
    prefixop = prefOp;
}
