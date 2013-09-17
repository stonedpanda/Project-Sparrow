//============================================================================
// Name        : console.cpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "console.hpp"

//
// Private Methods
//

//
// Public Methods
//

Console::Console() {
	running = true;
	s_mapCommandValues["create"] = cvCreateRequest;
	s_mapCommandValues["help"] = cvDisplayHelp;
	s_mapCommandValues["init"] = cvInitDirectory;
	s_mapCommandValues["list"] = cvListRequests;
	s_mapCommandValues["quit"] = cvQuitProgram;
	s_mapCommandValues["sha1sum"] = cvCalculateHash;
	s_mapCommandValues["sync"] = cvSyncDirectories;
	s_mapCommandValues["version"] = cvShowVersion;
}

void Console::run() {
	Methods aMethod;

	std::cout << "Project Sparrow | Offline File-Sharing Program\n" << std::endl;

	while(running) {
		std::cout << "Command: ";
		std::cout.flush();
		std::cin.getline(commandInput, 255);
		std::cout << std::endl;

		switch(s_mapCommandValues[commandInput]) {
            case cvCalculateHash:
                aMethod.sha1sum();
                break;
            case cvCreateRequest:
                aMethod.createRequest();
                break;
            case cvDisplayHelp:
                aMethod.help();
                break;
            case cvInitDirectory:
                aMethod.initDirectory();
                break;
            case cvListRequests:
                aMethod.listRequests();
                break;
            case cvShowVersion:
                aMethod.showVersion();
                break;
            case cvSyncDirectories:
                aMethod.sync();
                break;
            case cvQuitProgram:
                running = false;
                break;
            default:
                std::cout << "'" << commandInput << "' is not a valid command." << std::endl;
                std::cout << std::endl;
                std::cout << "Press enter to continue." << std::endl;
                break;
        }

        if(running) {
            std::cin.ignore(255, '\n');
        }
    }
}
