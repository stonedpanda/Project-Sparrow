//============================================================================
// Name        : console.hpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <iostream>
#include <map>

#include "methods.hpp"

class Console {
private:
	bool running;
	char commandInput[255];
	enum commandValues {
		cvNotDefined,
		cvCreateRequest,
		cvHelp,
		cvListRequests,
		cvQuit,
		cvSyncDirectories
	};
	std::map<std::string, commandValues> s_mapCommandValues;
public:
	Console();
	void run();
};

#endif /* CONSOLE_HPP_ */
