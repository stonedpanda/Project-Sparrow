//============================================================================
// Name        : console.hpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

// Load headers
#include "methods.hpp"

// Load 3rd-party libraries
#include <iostream>
#include <vector>

class Console {
    public:
        Console();
        int run(int, char**);
};

#endif /* CONSOLE_HPP_ */
