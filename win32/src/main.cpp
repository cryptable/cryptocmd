/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 11/08/2020
 */
#include <iostream>
#include "WebExtension.h"

int main(int argc, char* argv[]) {

    WebExtension webExtension(std::cin);

    webExtension.runFunction(std::cout);

    return 0;
}