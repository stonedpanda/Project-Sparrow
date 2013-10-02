//============================================================================
// Name        : methods.hpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef METHODS_HPP_
#define METHODS_HPP_

// Load headers
#include "crypto_library.hpp"
#include "file_registry.pb.h"
#include "request_registry.pb.h"
#include "search_index.pb.h"

// Load 3rd-party libraries
#include <boost/filesystem.hpp>
#include <ctime>
#include <fstream>

class Methods {
    private:
        bool exportFiles(std::string, std::string);
        bool exportIndexes(std::string, std::string);
        bool exportRequests(std::string, std::string);
        bool importFiles(std::string, std::string);
        bool importIndexes(std::string, std::string);
        bool importRequests(std::string, std::string);
        bool updateFileRegistry(std::string);
        bool updateRequestRegistry(std::string);
        bool updateSearchIndex(std::string);
    protected:
        bool findFile(std::string, std::string);
        bool listIndexes(std::string);
        bool listRequests(std::string);
    public:
        int createRequest(std::string, std::string);
        int initDirectory(std::string);
        int sha1sum(std::string);
        int showUsage(char**);
        int showVersion();
        int sync(std::string, std::string);
        void createRequest();
        void findFile();
        void help();
        void initDirectory();
        void listIndexes();
        void listRequests();
        void sha1sum();
        void sync();
};

#endif /* METHODS_HPP_ */
