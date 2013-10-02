//============================================================================
// Name        : console.cpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution-NonCommercial-ShareAlike License | http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
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
        } else if((arg == "-f") || (arg == "--find")) {
            if(i + 2 < argc) {
                return aMethod.findFile(argv[i + 1], argv[i + 2]);
            } else {
                std::cerr << "--find option requires two arguments." << std::endl;
                return 1;
            }
        } else if((arg == "-i") || (arg == "--init")) {
            if(i + 1 < argc) {
                return aMethod.initDirectory(argv[i + 1]);
            } else {
                std::cerr << "--init option requires one argument." << std::endl;
                return 1;
            }
        } else if((arg == "-l") || (arg == "--list")) {
            if(i + 2 < argc) {
                std::string type = argv[i + 2];
                if(type == "INDEXES") {
                    return aMethod.listIndexes(argv[i + 1]);
                } else if(type == "REQUESTS") {
                    return aMethod.listRequests(argv[i + 1]);
                } else {
                    std::cerr << "--list option requires a valid type (INDEXES, REQUESTS)" << std::endl;
                }
            } else {
                std::cerr << "--list option requires two arguments." << std::endl;
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
