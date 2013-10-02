//============================================================================
// Name        : crypto_library.hpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution-NonCommercial-ShareAlike License | http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CRYPTO_LIBRARY_HPP_
#define CRYPTO_LIBRARY_HPP_

// Load 3rd-party libraries
#include "sha1.h"
#include <fstream>
#include <string>
//#include <crypto++/dll.h>
//#include <crypto++/files.h>
//#include <crypto++/hex.h>
//#include <crypto++/sha.h>

class CryptoLibrary {
    private:
    public:
        CryptoLibrary();
        std::string sha1sum(std::string);
};

#endif /* CRYPTO_HPP_ */
