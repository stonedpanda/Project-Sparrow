//============================================================================
// Name        : methods.hpp
// Author      : Justin Holz
// Version     : 0.3
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef METHODS_HPP_
#define METHODS_HPP_

// Load headers
#include "crypto.hpp"
#include "file_registry.pb.h"
#include "request_registry.pb.h"
#include "search_index.pb.h"

// Load 3rd-party libraries
#include <boost/filesystem.hpp>
#include <crypto++/files.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>
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
        bool createRequest(std::string, std::string);
        bool findFile(std::string);
        bool initDirectory(std::string);
        bool listIndexes(std::string);
        bool listRequests(std::string);
        bool sha1sum(std::string);
        bool sync(std::string, std::string);
    public:
        void createRequest();
        void findFile();
        void help();
        void initDirectory();
        void listIndexes();
        void listRequests();
        void sha1sum();
        void showVersion();
        void sync();
};

#endif /* METHODS_HPP_ */
