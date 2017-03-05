//
//  main.cpp
//  groho
//
//  Created by Kaushik Ghose on 3/4/17.
//  Copyright © 2017 Bengal Bionics. All rights reserved.
//

#include <iostream>
#include "cli.hpp"

int main(int argc, char * argv[]) {
    Options opts = parse_options(argc, argv);
    std::cout << opts << std::endl;
    return 0;
}
