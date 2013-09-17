//============================================================================
// Name        : console.cpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load libraries
#include "console.hpp"

//
// Private Methods
//

//
// Public Methods
//

Console::Console() {
	running = true;
	s_mapCommandValues["createRequest"] = cvCreateRequest;
	s_mapCommandValues["help"] = cvHelp;
	s_mapCommandValues["listRequests"] = cvListRequests;
	s_mapCommandValues["quit"] = cvQuit;
	s_mapCommandValues["syncDirectories"] = cvSyncDirectories;
}

void Console::run() {
	Methods aMethod;

	std::cout << "Project Sparrow | Offline File-Sharing Program\n" << std::endl;

	while(running) {
		std::cout << "Command: ";
		std::cout.flush();
		std::cin.getline(commandInput, 255);

		switch(s_mapCommandValues[commandInput]) {
            case cvCreateRequest:
                aMethod.createRequest();
                break;
            case cvHelp:
                aMethod.help();
                break;
            case cvListRequests:
                aMethod.listRequests();
                break;
            case cvQuit:
                running = false;
                return;
            case cvSyncDirectories:
                aMethod.syncDirectories();
                break;
            default:
                std::cout << "'" << commandInput << "' is not a valid command. ";
                std::cout << "Press enter to continue." << std::endl;
                break;
        }

        std::cin.ignore(255, '\n');
    }
}
