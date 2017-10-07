//
// Created by roman sztergbaum on 27/09/2017.
//

#include <iostream>
#include "Configuration.hpp"
#include "CSpiderCore.hpp"

int main()
{
    try {
        spi::Configuration::initialize();
        spi::CSpiderCore core;

        core.run();
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}