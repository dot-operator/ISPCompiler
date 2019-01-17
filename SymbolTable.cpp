//
//  SymbolTable.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/16/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include "SymbolTable.hpp"

void SymbolTable::incScope(){
    ++scope;
}

void SymbolTable::decScope(){
    --scope;
    unsigned symScope = 0;
    do {
        auto& [name, type, depth] = symbols.back();
        symScope = depth;
        if(symScope > scope)
            symbols.pop_back();
    } while (symScope > scope);
}

void SymbolTable::addSymbol(const Symbol &sym) {
    symbols.push_back(sym);
}

void SymbolTable::addSymbol(const string &name, const string &type){
    symbols.push_back(std::tie(name, type, scope));
}

Symbol* SymbolTable::find(const string &name, const string& type){
    for(auto rit = symbols.rbegin(); rit != symbols.rend(); ++rit){
        auto& [sname, stype, depth] = *rit;
        if(sname == name){
            if(type == "" || type == stype)
                return &*rit;
        }
    }
    return nullptr;
}

SymbolTable::SymbolTable(){
    
}

SymbolTable::~SymbolTable(){
    
}
