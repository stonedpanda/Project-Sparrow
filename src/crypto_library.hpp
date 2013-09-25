//============================================================================
// Name        : crypto_library.hpp
// Author      : Justin Holz
// Version     : 0.3
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CRYPTO_LIBRARY_HPP_
#define CRYPTO_LIBRARY_HPP_

// Load 3rd-party libraries
#include <crypto++/dll.h>
#include <crypto++/files.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>

class CryptoLibrary {
    private:
    public:
        CryptoLibrary();
        std::string sha1sum(std::string);
};

#endif /* CRYPTO_HPP_ */
