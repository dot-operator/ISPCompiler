//
//  SymbolTable.hpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/16/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#ifndef SymbolTable_hpp
#define SymbolTable_hpp

#include <string>
#include <tuple>
#include <vector>

using std::string;
// First string is name, second is type, int is scope/depth.
typedef std::tuple<string, string, unsigned int, bool> Symbol;

class SymbolTable {
private:
    std::vector<Symbol> symbols;
    std::vector<Symbol> parameters;
    unsigned scope = 0;
public:
    SymbolTable();
    ~SymbolTable();
    
    void incScope();
    void decScope();
    
    void addSymbol(const Symbol& sym);
    void addSymbol(const string& name, const string& type, bool function);
    // addParameter doesn't add to the symbol table until the scope increases.
    void addParameter(const string& name, const string& type);
    
    // Might not find anything- can return null!
    Symbol* find(const string& name, const string& type = "");
};

#endif /* SymbolTable_hpp */
