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
    
    for(auto& param : parameters){
        SymbolEntry entry;
        entry.sym = param;
        symbols.push_back(entry);
    }
    parameters.clear();
}

void SymbolTable::decScope(){
    --scope;
    unsigned symScope = 0;
    do {
        auto& [name, type, depth, func] = symbols.back().sym;
        symScope = depth;
        if(symScope > scope)
            symbols.pop_back();
    } while (symScope > scope);
}

void SymbolTable::addSymbol(const Symbol &sym) {
    SymbolEntry entry;
    entry.sym = sym;
    symbols.push_back(entry);
}

void SymbolTable::addSymbol(const string &name, const string &type, bool function){
    SymbolEntry entry;
    entry.sym = std::tie(name, type, scope, function);
    symbols.push_back(entry);
}

void SymbolTable::addParameter(const string &name, const string &type){
    bool function = false;
    int nScope = scope + 1;
    parameters.push_back(std::tie(name, type, nScope, function));
}

Symbol* SymbolTable::find(const string &name, const string& type){
    for(auto rit = symbols.rbegin(); rit != symbols.rend(); ++rit){
        auto& [sname, stype, depth, func] = (*rit).sym;
        if(sname == name){
            if(type == "" || type == stype)
                return &(*rit).sym;
        }
    }
    return nullptr;
}

SymbolTable::SymbolTable(){
    
}

SymbolTable::~SymbolTable(){
    
}
