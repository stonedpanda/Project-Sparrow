/*
 * console.hpp
 *
 *  Created on: Aug 20, 2012
 *      Author: stonedpanda
 */

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
