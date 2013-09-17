//============================================================================
// Name        : crypto.hpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#ifndef CRYPTO_HPP_
#define CRYPTO_HPP_

// Load 3rd-party libraries
#include <crypto++/dll.h>
#include <crypto++/files.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>

class Crypto {
    private:
    public:
        Crypto();
        std::string sha1sum(std::string);
};

#endif /* CRYPTO_HPP_ */
