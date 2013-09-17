//============================================================================
// Name        : crypto.cpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "crypto.hpp"

//
// Private Methods
//

//
// Public Methods
//

Crypto::Crypto() {
}

std::string Crypto::sha1sum(std::string path) {
    CryptoPP::SHA1 hashFunction;
    std::string digest;
    CryptoPP::FileSource(
        path.c_str(),
        true,
        new CryptoPP::HashFilter(
            hashFunction,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(
                    digest
                ),
                false
            )
        )
    );
    return digest;
}
