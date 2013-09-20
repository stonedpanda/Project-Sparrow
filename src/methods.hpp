//============================================================================
// Name        : methods.hpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef METHODS_HPP_
#define METHODS_HPP_

// Load headers
#include "crypto.hpp"
#include "file_registry.pb.h"
#include "request_registry.pb.h"

// Load 3rd-party libraries
#include <boost/filesystem.hpp>
#include <crypto++/files.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>
#include <ctime>
#include <fstream>

class Methods {
    private:
        std::string createRequest(std::string, std::string);
        void exportFiles(std::string, std::string);
        void exportIndexes(std::string, std::string);
        void exportRequests(std::string, std::string);
        void importFiles(std::string, std::string);
        void importIndexes(std::string, std::string);
        void importRequests(std::string, std::string);
        void initDirectory(std::string);
        void listRequests(std::string);
        void sync(std::string, std::string);
        void updateFileRegistry(std::string);
        void updateRequestRegistry(std::string);
        void updateSearchIndex(std::string);
    public:
        void createRequest();
        void help();
        void initDirectory();
        void listRequests();
        void sha1sum();
        void showVersion();
        void sync();
};

#endif /* METHODS_HPP_ */
