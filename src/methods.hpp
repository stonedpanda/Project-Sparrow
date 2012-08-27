/*
 * methods.hpp
 *
 *  Created on: Aug 20, 2012
 *      Author: stonedpanda
 */

#ifndef METHODS_HPP_
#define METHODS_HPP_

#include <boost/filesystem.hpp>
#include <crypto++/files.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>
#include <ctime>
#include <fstream>

#include "file_registry.pb.h"
#include "request_registry.pb.h"

class Methods {
private:
	void exportFiles(std::string, std::string);
	void importFiles(std::string, std::string);
	void exportRequests(std::string, std::string);
	void importRequests(std::string, std::string);
	void initDirectory(std::string);
	void updateFileRegistry(std::string);
	void updateRequestRegistry(std::string);
public:
	void createRequest();
	void help();
	void listRequests();
	void syncDirectories();
};

#endif /* METHODS_HPP_ */
