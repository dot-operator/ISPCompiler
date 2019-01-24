//
//  main.cpp
//  ISPCompiler
//
//  Created by Sullivan Boyd on 1/9/19.
//  Copyright © 2019 Sullivan Boyd. All rights reserved.
//

#include <iostream>
#include "Parser.hpp"

int main(int argc, const char * argv[]) {
    Parser parser("test_ir.c");
    parser.prettyPrint();
    parser.makeIR("test_ir.ir");
    return 0;
}
