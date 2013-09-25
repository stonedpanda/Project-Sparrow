//============================================================================
// Name        : console.hpp
// Author      : Justin Holz
// Version     : 0.3
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

// Load headers
#include "methods.hpp"

// Load 3rd-party libraries
#include <iostream>
#include <map>

class Console {
    private:
        bool running;
        char commandInput[255];
        enum commandValues {
            cvNotDefined,
            cvCalculateHash,
            cvCreateRequest,
            cvDisplayHelp,
            cvFindFile,
            cvInitDirectory,
            cvListIndexes,
            cvListRequests,
            cvQuitProgram,
            cvShowVersion,
            cvSyncDirectories,
        };
        std::map<std::string, commandValues> s_mapCommandValues;
    public:
        Console();
        void run();
};

#endif /* CONSOLE_HPP_ */
