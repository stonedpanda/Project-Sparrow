//============================================================================
// Name        : crypto_library.cpp
// Author      : Justin Holz
// Version     : 0.4
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "crypto_library.hpp"

//
// Private Methods
//

//
// Public Methods
//

CryptoLibrary::CryptoLibrary() {
}

std::string CryptoLibrary::sha1sum(std::string path) {
    // Initialize variables
    std::string digest;

    // Load file
    std::ifstream t(path.c_str());
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);

    unsigned char hash[20];
    char hexstring[41];
    sha1::calc(buffer.c_str(), buffer.size(), hash);
    sha1::toHexString(hash, hexstring);

    digest = hexstring;

    return digest;
}
