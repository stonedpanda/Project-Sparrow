//============================================================================
// Name        : console.cpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "console.hpp"

//
// Public Methods
//

Console::Console() {
}

int Console::run(int argc, char** argv) {
	Methods aMethod;
	std::vector <std::string> sources;

	for(int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if((argc == 1) || (arg == "-h") || (arg == "--help")) {
            return aMethod.showUsage(argv);
        } else if((arg == "-d") || (arg == "--digest")) {
            if(i + 1 < argc) {
                return aMethod.sha1sum(argv[i + 1]);
            } else {
                std::cerr << "--digest option requires one argument." << std::endl;
                return 1;
            }
        } else if((arg == "-i") || (arg == "--init")) {
            if(i + 1 < argc) {
                return aMethod.initDirectory(argv[i + 1]);
            } else {
                std::cerr << "--init option requires one argument." << std::endl;
                return 1;
            }
        } else if((arg == "-r") || (arg == "--request")) {
            if(i + 2 < argc) {
                return aMethod.createRequest(argv[i + 1], argv[i + 2]);
            } else {
                std::cerr << "--request option requires two arguments." << std::endl;
                return 1;
            }
        } else if((arg == "-s") || (arg == "--sync")) {
            if(i + 2 < argc) {
                return aMethod.sync(argv[i + 1], argv[i + 2]);
            } else {
                std::cerr << "--sync option requires two arguments." << std::endl;
                return 1;
            }
        } else if((arg == "-v") || (arg == "--version")) {
            return aMethod.showVersion();
        } else {
            sources.push_back(argv[i]);
        }
    }
    return 1;
}
