//
//  main.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/9/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include <iostream>
#include "LexicalAnalyzer.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    LexicalAnalyzer lexer;
    lexer.loadFile("test.c");
    while(lexer.hasNext()){
        std::cout << (int)lexer.getNext().type << "\n";
    }
    return 0;
}
