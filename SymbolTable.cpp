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
        symbols.push_back(param);
    }
    parameters.clear();
}

void SymbolTable::decScope(){
    --scope;
    unsigned symScope = 0;
    do {
        auto& [name, type, depth, func, parms, uses] = symbols.back();
        symScope = depth;
        if(symScope > scope)
            symbols.pop_back();
    } while (symScope > scope);
}

void SymbolTable::addSymbol(Symbol sym) {
    sym.scope = scope;
    symbols.push_back(sym);
}

void SymbolTable::addSymbol(const string &name, const string &type, bool function){
    Symbol sym(name, type, scope, function);
    symbols.push_back(sym);
}

void SymbolTable::addParameter(const string &name, const string &type){
    bool function = false;
    int nScope = scope + 1;
    
    Symbol sym(name, type, nScope, function);
    parameters.push_back(sym);
}

Symbol* SymbolTable::find(const string &name, const string& type){
    for(auto rit = symbols.rbegin(); rit != symbols.rend(); ++rit){
        auto& [sname, stype, depth, func, parms, uses] = *rit;
        if(sname == name){
            if(type == "" || type == stype)
                return &(*rit);
        }
    }
    return nullptr;
}

SymbolTable::SymbolTable(){
    
}

SymbolTable::~SymbolTable(){
    
}
