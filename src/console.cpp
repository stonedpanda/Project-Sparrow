/*
 * console.cpp
 *
 *  Created on: Aug 20, 2012
 *      Author: stonedpanda
 */

#include "console.hpp"

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
