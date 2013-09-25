//============================================================================
// Name        : methods.cpp
// Author      : Justin Holz
// Version     : 0.3
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "methods.hpp"

//
// Private Methods
//

bool Methods::exportFiles(std::string local_root, std::string portable_root) {
    // Instantiate variables
    bool updateRequestRegistry;
    Crypto aCrypto;
    file_registry::File *aFile;
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;
	request_registry::Request *aRequest;
	std::string actual_hash, file_path, fr_filename, fr_path, rr_filename, rr_path;

	// Set parent variables
    updateRequestRegistry = false;
	fr_filename = "file_registry.proto";
	rr_filename = "request_registry.proto";


	// Set child variables
    fr_path = local_root + "/" + fr_filename;
	rr_path = portable_root + "/" + rr_filename;

	// Load file registry
	std::fstream fr_input(fr_path.c_str(), std::ios::in | std::ios::binary);
    if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Error: Failed to load file registry" << std::endl;
		return false;
	}
	fr_input.close();

	// Load request registry
	std::fstream rr_input(rr_path.c_str(), std::ios::in | std::ios::binary);
	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Error: Failed to load request registry" << std::endl;
		return false;
	}
	rr_input.close();

	for(int i = 0; i < aRequestRegistry.request_size(); i++) {
		aRequest = aRequestRegistry.mutable_request(i);

		// Check if request is active
		if(aRequest->active()) {
			for(int j = 0; j < aFileRegistry.file_size(); j++) {
				aFile = aFileRegistry.mutable_file(j);
				if(aFile->hash() == aRequest->hash()) {
					actual_hash = aCrypto.sha1sum(local_root + "/" + aFile->path());
					if(aFile->hash() == actual_hash) {
						boost::filesystem::copy_file(
							local_root + aFile->path(),
							portable_root + "/shared/" + aFile->name(),
							boost::filesystem::copy_option::overwrite_if_exists
						);
						aRequest->set_active(false);
						updateRequestRegistry = true;
					} else {
						std::cerr << "Error: Invalid hash." << std::endl;
					}
					break;
				}
			}
		}
	}

	if(updateRequestRegistry) {
        // Update request registry
		std::fstream output(rr_path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!aRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Error: Failed to update request registry." << std::endl;
			return false;
		}
		output.close();
		updateFileRegistry(portable_root);
	}
    return true;
}

bool Methods::exportIndexes(std::string local_root, std::string portable_root) {
    // Declare variables
    bool fileFound = false;
	bool updatePortable = false;
    search_index::Index localSearchIndex, portableSearchIndex;
    std::string local_file = local_root + "/search_index.proto";
    std::string portable_file = portable_root + "/search_index.proto";

	// Load local search index
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
    if(!localSearchIndex.ParseFromIstream(&local_input)) {
        std::cerr << "Error: Failed to load local search index." << std::endl;
		return false;
	}
	local_input.close();

	// Load portable search index
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
    if(!portableSearchIndex.ParseFromIstream(&portable_input)) {
        std::cerr << "Error: Failed to load portable search index." << std::endl;
		return false;
	}
	portable_input.close();

    for(int i = 0; i < localSearchIndex.file_size(); i++) {
        search_index::File *aLocalFile = localSearchIndex.mutable_file(i);
        fileFound = false;

        for(int j = 0; j < portableSearchIndex.file_size(); j++) {
            const search_index::File& aPortableFile = portableSearchIndex.file(i);
            if(aLocalFile->hash() == aPortableFile.hash()) {
                    fileFound = true;

            }

        }

        if(!fileFound) {
            search_index::File *aFile = portableSearchIndex.add_file();
            aFile->set_hash(aLocalFile->hash());
            aFile->set_name(aLocalFile->name());
            aFile->set_size(aLocalFile->size());
            aFile->set_type(aLocalFile->type());
            updatePortable = true;

        }
	}

	if(updatePortable) {
        // Update portable search index
		std::fstream output(portable_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        if(!portableSearchIndex.SerializeToOstream(&output)) {
            std::cerr << "Error: Failed to update search index." << std::endl;
            return false;
	    }
	    output.close();
	}
    return true;
}

bool Methods::exportRequests(std::string local_root_dir, std::string portable_root_dir) {
    // Declare variables
	bool requestFound = false;
	bool updatePortable = false;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file = local_root_dir + "/request_registry.proto";
	std::string portable_file = portable_root_dir + "/request_registry.proto";

	// Load local request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Error: Failed to load local request registry." << std::endl;
		return false;
	}
	local_input.close();

	// Load portable request registry
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
	if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Error: Failed to load portable request registry." << std::endl;
		return false;
	}
	portable_input.close();

	for(int i = 0; i< localRequestRegistry.request_size(); i++) {
		request_registry::Request *aLocalRequest = localRequestRegistry.mutable_request(i);
		requestFound = false;

		if(aLocalRequest->active()) {
			for(int j = 0; j < portableRequestRegistry.request_size(); j++) {
				const request_registry::Request& aPortableRequest = portableRequestRegistry.request(j);
				if(aLocalRequest->hash() == aPortableRequest.hash()) {
					requestFound = true;
				}
			}

			if(!requestFound) {
				request_registry::Request *aRequest = portableRequestRegistry.add_request();
				aRequest->set_active(true);
				aRequest->set_hash(aLocalRequest->hash());
				aRequest->set_timeout(aLocalRequest->timeout());
				updatePortable = true;
			}
		}
	}

	if(updatePortable) {
        // Update portable request registry
		std::fstream output(portable_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!portableRequestRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Error: Failed to update request registry." << std::endl;
            return false;
	    }
	    output.close();
	}
    return true;
}

bool Methods::importFiles(std::string local_root, std::string portable_root) {
    // Declare variables
    bool updateRequestRegistry = false;
    Crypto aCrypto;
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;
	request_registry::Request *aRequest;
	std::string actual_hash, file_path;
    std::string fr_file = portable_root + "/file_registry.proto";
	std::string rr_file = local_root + "/request_registry.proto";

    // Load file registry
	std::fstream fr_input(fr_file.c_str(), std::ios::in | std::ios::binary);
    if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Error: Failed to load file registry." << std::endl;
		return false;
	}
	fr_input.close();

	// Load request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);
    if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Error: Failed to load request registry." << std::endl;\
		return false;
	}
    rr_input.close();

	for(int i = 0; i< aRequestRegistry.request_size(); i++) {
		aRequest = aRequestRegistry.mutable_request(i);

		if(aRequest->active()) {
			for(int j = 0; j < aFileRegistry.file_size(); j++) {
				const file_registry::File& aFile = aFileRegistry.file(j);
				if(aFile.hash() == aRequest->hash()) {
					actual_hash = aCrypto.sha1sum(portable_root + aFile.path());

					if(aFile.hash() == actual_hash) {
						boost::filesystem::copy_file(
							portable_root + aFile.path(),
							local_root + "/shared/" + aFile.name(),
							boost::filesystem::copy_option::overwrite_if_exists
						);
						aRequest->set_active(false);
						updateRequestRegistry = true;
					} else {
						std::cerr << "Error: Invalid hash." << std::endl;
					}
				}
			}
		}
	}

	if(updateRequestRegistry) {
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!aRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Error: Failed to update request registry." << std::endl;
			return false;
		}
		output.close();
		updateFileRegistry(local_root);
	}
    return true;
}

bool Methods::importIndexes(std::string local_root, std::string portable_root) {
    // Declare variables
    bool fileFound = false;
	bool updateLocal = false;
    search_index::Index localSearchIndex, portableSearchIndex;
    std::string local_file = local_root + "/search_index.proto";
    std::string portable_file = portable_root + "/search_index.proto";

	// Load local search index
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
    if(!localSearchIndex.ParseFromIstream(&local_input)) {
        std::cerr << "Error: Failed to load local search registry." << std::endl;
		return false;
	}
	local_input.close();

	// Load portable search index
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
    if(!portableSearchIndex.ParseFromIstream(&portable_input)) {
        std::cerr << "Error: Failed to load portable search index." << std::endl;
		return false;
	}
	portable_input.close();

    for(int i = 0; i < portableSearchIndex.file_size(); i++) {
        search_index::File *aPortableFile = portableSearchIndex.mutable_file(i);
        fileFound = false;

        for(int j = 0; j < localSearchIndex.file_size(); j++) {
            const search_index::File& aLocalFile = localSearchIndex.file(j);
            if(aLocalFile.hash() == aPortableFile->hash()) {
                fileFound = true;
            }
        }

        if(!fileFound) {
            updateLocal = true;
            search_index::File *aFile = localSearchIndex.add_file();
            aFile->set_hash(aPortableFile->hash());
            aFile->set_name(aPortableFile->name());
            aFile->set_size(aPortableFile->size());
            aFile->set_type(aPortableFile->type());
        }
	}

	if(updateLocal) {
        // Update local search index
		std::fstream output(local_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        if(!localSearchIndex.SerializePartialToOstream(&output)) {
            std::cerr << "Error: Failed to update search index." << std::endl;
			return false;
		}
		output.close();
	}
	return true;
}

bool Methods::importRequests(std::string local_root_dir, std::string portable_root_dir) {
    // Declare variables
	bool requestFound = false;
	bool updateLocal = false;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file = local_root_dir + "/request_registry.proto";
	std::string portable_file = portable_root_dir + "/request_registry.proto";

	// Load local request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Error: Failed to load local request registry." << std::endl;
		return false;
	}
	local_input.close();

	// Load portable request registry
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
    if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Error: Failed to load portable request registry." << std::endl;
		return false;
	}
	portable_input.close();

	for(int i = 0; i < portableRequestRegistry.request_size(); i++) {
		request_registry::Request *aPortableRequest = portableRequestRegistry.mutable_request(i);
		requestFound = false;

		if(aPortableRequest->active()) {
			for(int j = 0; j < localRequestRegistry.request_size(); j++) {
				const request_registry::Request& aLocalRequest = localRequestRegistry.request(j);
				if(aLocalRequest.hash() == aPortableRequest->hash()) {
					requestFound = true;
				}
			}

			if(!requestFound) {
				updateLocal = true;
				request_registry::Request *aRequest = localRequestRegistry.add_request();
				aRequest->set_active(true);
				aRequest->set_hash(aPortableRequest->hash());
				aRequest->set_timeout(aPortableRequest->timeout());
			}
		}
	}

	if(updateLocal) {
        // Update local request registry
		std::fstream output(local_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!localRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Error: Failed to update request registry." << std::endl;
			return false;
		}
		output.close();
	}
	return true;
}

bool Methods::updateFileRegistry(std::string root_directory) {
    // Declare variables
	bool alreadyDiscovered, fileDiscovered;
	Crypto aCrypto;
	file_registry::Registry aFileRegistry;
	std::string registry_file = root_directory + "/file_registry.proto";
	std::string share_directory = root_directory + "/shared";

	boost::filesystem::path share_path(share_directory);
	boost::filesystem::directory_iterator iter(share_path), end;

	// Load file registry
	std::fstream fr_input(registry_file.c_str(), std::ios::in | std::ios::binary);
	if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Error: Failed to load file registry." << std::endl;
		return false;
	}
	fr_input.close();

	// Lookup each file in shared directory
	for(;iter != end; ++iter) {
		alreadyDiscovered = false;
		std::string fileName = iter->path().filename().string();
		std::string filePath = "/shared/" + fileName;
		std::string fileHash = aCrypto.sha1sum(share_directory + "/" + fileName);

		// Look up hash in file registry
		for(int i = 0; i < aFileRegistry.file_size(); i++) {
			const file_registry::File& aFile = aFileRegistry.file(i);
			if(aFile.hash() == fileHash) {
				alreadyDiscovered = true;
				break;
			}
		}

		if(!alreadyDiscovered) {
			std::cout << fileHash + ": just discovered" << std::endl;
			fileDiscovered = true;
			file_registry::File *aFile = aFileRegistry.add_file();
			aFile->set_active(true);
			aFile->set_hash(fileHash);
			aFile->set_name(fileName);
			aFile->set_path(filePath);
		} else {
//			std::cout << fileHash + ": already discovered" << std::endl;
		}
	}

	if(fileDiscovered) {
		std::fstream output(registry_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!aFileRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Error: Failed to update file registry." << std::endl;
	    	return false;
	    }
	    output.close();
	}
	return true;
}

bool Methods::updateRequestRegistry(std::string root_directory) {
    // Declare variables
	request_registry::Registry newRequestRegistry;
	request_registry::Registry oldRequestRegistry;
    std::string rr_file = root_directory + "/request_registry.proto";

    // Load request registry
	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!oldRequestRegistry.ParseFromIstream(&input)) {
		std::cerr << "Error: Unable to load request registry" << std::endl;
	}
	input.close();

	for(int i = 0; i < oldRequestRegistry.request_size(); i++) {
		std::time_t t = std::time(0);
		request_registry::Request *aRequest = oldRequestRegistry.mutable_request(i);
		if(aRequest->timeout() > t) {
			request_registry::Request *newRequest = newRequestRegistry.add_request();
			newRequest->set_active(aRequest->active());
			newRequest->set_hash(aRequest->hash());
			newRequest->set_timeout(aRequest->timeout());
		}
	}

    // Update request registry
	std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!newRequestRegistry.SerializeToOstream(&output)) {
    	std::cerr << "Error: Failed to update request registry." << std::endl;
    	return false;
    }
    output.close();
    return true;
}

bool Methods::updateSearchIndex(std::string root_directory) {
    // Declare variables
	bool alreadyDiscovered, fileDiscovered;
	Crypto aCrypto;
    search_index::Index aSearchIndex;
    std::string index_file = root_directory + "/search_index.proto";
	std::string share_directory = root_directory + "/shared";

	boost::filesystem::path share_path(share_directory);
	boost::filesystem::directory_iterator iter(share_path), end;

	// Load search index
    std::fstream si_input(index_file.c_str(), std::ios::in | std::ios::binary);
    if(!aSearchIndex.ParseFromIstream(&si_input)) {
        std::cerr << "Error: Failed to load search index." << std::endl;
		return false;
	}
    si_input.close();

	// Lookup each file in shared directory
	for(;iter != end; ++iter) {
		alreadyDiscovered = false;
		std::string fileName = iter->path().filename().string();
		std::string filePath = "/shared/" + fileName;
		std::string fileHash = aCrypto.sha1sum(share_directory + "/" + fileName);
		int fileSize = boost::filesystem::file_size(share_directory + "/" + fileName);
		std::string fileType = boost::filesystem::extension(filePath);

		boost::filesystem::path aPath(share_directory + "/" + fileName);
		std::string stem = aPath.stem().string();

		// Look up hash in search index
        for(int i = 0; i < aSearchIndex.file_size(); i++) {
            const search_index::File& aFile = aSearchIndex.file(i);
			if(aFile.hash() == fileHash) {
				alreadyDiscovered = true;
				break;
			}
		}

		if(!alreadyDiscovered) {
			std::cout << fileHash + ": just discovered" << std::endl;
			fileDiscovered = true;
            search_index::File *aFile = aSearchIndex.add_file();
			aFile->set_hash(fileHash);
			aFile->set_name(stem);
            aFile->set_size(fileSize);
            aFile->set_type(fileType);
		} else {
//			std::cout << fileHash + ": already discovered" << std::endl;
		}
    }

    if(fileDiscovered) {
        std::fstream output(index_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        if(!aSearchIndex.SerializeToOstream(&output)) {
            std::cerr << "Error: Failed to update search index." << std::endl;
            return false;
        }
        output.close();
	}
	return true;
}

//
// Protected Methods
//

bool Methods::createRequest(std::string file_hash, std::string root_directory) {
	// Declare variables
	bool alreadyRequested = false;
	bool requestAdded = false;
	request_registry::Registry aRequestRegistry;
	std::string rr_file = root_directory + "/request_registry.proto";

	// Load request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Error: Failed to load request registry." << std::endl;
		return false;
	}
	rr_input.close();

	for(int i = 0; i< aRequestRegistry.request_size(); i++) {
		const request_registry::Request& aRequest = aRequestRegistry.request(i);
		if(aRequest.hash() == file_hash) {
			alreadyRequested = true;
		}
	}

	if(!alreadyRequested) {
		std::time_t t = std::time(0);
		int timeout = t + 1209600;	// 2 weeks
		requestAdded = true;
		request_registry::Request *aRequest = aRequestRegistry.add_request();
		aRequest->set_active(true);
		aRequest->set_hash(file_hash);
		aRequest->set_timeout(timeout);
	} else {
//		std::cout << file_hash + ": already requested" << std::endl;
	}

	if(requestAdded) {
        // Update request registry
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!aRequestRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Error: Failed to update request registry." << std::endl;
	    	return false;
	    }
	    output.close();
	}
	return true;
}

bool Methods::findFile(std::string query) {
    return true;
}

bool Methods::initDirectory(std::string root_directory) {
    // Declare variables
    FILE *pFile;
	std::string fr_file = root_directory + "/file_registry.proto";
	std::string rr_file = root_directory + "/request_registry.proto";
	std::string si_file = root_directory + "/search_index.proto";
	std::string share_folder = root_directory + "/shared";

    // Check for file registry
	if(!boost::filesystem::exists(fr_file)) {
        // Create file registry
		pFile = fopen(fr_file.c_str(), "w");
		if(pFile != NULL) {
			fclose(pFile);
		}
	}

    // Check for request registry
	if(!boost::filesystem::exists(rr_file)) {
        // Create request registry
		pFile = fopen(rr_file.c_str(), "w");
		if(pFile != NULL) {
			fclose(pFile);
		}
	}

	// Check for search index
	if(!boost::filesystem::exists(si_file)) {
        // Create search index
        pFile = fopen(si_file.c_str(), "w");
        if(pFile != NULL) {
            fclose(pFile);
        }
	}

    // Check for share folder
	if(!boost::filesystem::is_directory(share_folder)) {
        // Create share folder
		boost::filesystem::create_directory(share_folder);
	}
	return true;
}

bool Methods::listIndexes(std::string root_directory) {
    // Declare variables
//	request_registry::Registry aRequestRegistry;
    search_index::Index aSearchIndex;
//	std::string rr_file = root_directory + "/request_registry.proto";
    std::string si_file = root_directory + "/search_index.proto";

    // Load request registry
//	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);
    std::fstream input(si_file.c_str(), std::ios::in | std::ios::binary);
//	if(!aRequestRegistry.ParseFromIstream(&input)) {
    if(!aSearchIndex.ParseFromIstream(&input)) {
//		std::cerr << "Error: Unable to load request registry." << std::endl;
        std::cerr << "Error: Unable to load search index." << std::endl;
		return false;
	}
	input.close();

//	for(int i = 0; i < aRequestRegistry.request_size(); i++) {
    for(int i = 0; i < aSearchIndex.file_size(); i++) {
//		const request_registry::Request &aRequest = aRequestRegistry.request(i);
        const search_index::File &aFile = aSearchIndex.file(i);
//		std::cout << "Request - " << aRequest.hash() << " - " << aRequest.active() << " - " << aRequest.timeout() << std::endl;
        std::cout << "File - " << aFile.hash() << " - " << aFile.name() << " - " << aFile.size() << " - " << aFile.type() << std::endl;
	}
	return true;
}

bool Methods::listRequests(std::string root_directory) {
    // Declare variables
	request_registry::Registry aRequestRegistry;
	std::string rr_file = root_directory + "/request_registry.proto";

    // Load request registry
	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!aRequestRegistry.ParseFromIstream(&input)) {
		std::cerr << "Error: Unable to load request registry." << std::endl;
		return false;
	}
	input.close();

	for(int i = 0; i < aRequestRegistry.request_size(); i++) {
		const request_registry::Request &aRequest = aRequestRegistry.request(i);
		std::cout << "Request - " << aRequest.hash() << " - " << aRequest.active() << " - " << aRequest.timeout() << std::endl;
	}
	return true;
}

bool Methods::sha1sum(std::string path) {
    //Declare variables
    Crypto aCrypto;

    std::cout << aCrypto.sha1sum(path) << std::endl;
    return true;
}

bool Methods::sync(std::string local_root, std::string portable_root) {
    // Initialize directories
    std::cout << "Initializing directories... ";
    if(initDirectory(local_root) && initDirectory(portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    // Update request registries
    std::cout << "Updating request registries... ";
    if(updateRequestRegistry(local_root) && updateRequestRegistry(portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    // Update file registries
    std::cout << "Updating file registries... ";
    if(updateFileRegistry(local_root) && updateFileRegistry(portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    // Updating search registries
    std::cout << "Updating search indexes... ";
    if(updateSearchIndex(local_root) && updateSearchIndex(portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    // Sync files
    std::cout << "Syncing files... ";
    if(exportFiles(local_root, portable_root) && importFiles(local_root, portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    // Sync requests
    std::cout << "Syncing requests... ";
    if(exportRequests(local_root, portable_root) && importRequests(local_root, portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
        return false;
    }

    std::cout << "Syncing indexes... ";
    if(exportIndexes(local_root, portable_root) && importIndexes(local_root, portable_root)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
    }

    return true;
}

//
// Public Methods
//

void Methods::createRequest() {
    // Declare variables
    std::string digest, directory;

    // Ask user for directory
    std::cout << "Enter directory: ";
    std::cin >> directory;
    std::cout << std::endl;

    // Ask user for digest
    std::cout << "Enter file hash: ";
    std::cin >> digest;
    std::cout << std::endl;

    // Create request
    std::cout << "Creating request... ";
    if(createRequest(digest, directory)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::findFile() {
    // Declare variables
    std::string query;

    // Ask user for query
    std::cout << "Enter query: ";
    std::cin >> query;
    std::cout << std::endl;

    // Executing search query
    std::cout << "Finding file... ";
    if(findFile(query)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::help() {
    std::cout << "create - Create a file request." << std::endl;
    std::cout << "init - Initialize a directory." << std::endl;
    std::cout << "list - List file requests in registry." << std::endl;
    std::cout << "quit - Exit the program." << std::endl;
    std::cout << "sha1sum - Calculate the hash of a file." << std::endl;
    std::cout << "sync - Sync files and requests between two directories." << std::endl;
    std::cout << "version - Show version." << std::endl;
    std::cout << std::endl;
    std::cout << "Press enter to continue." << std::endl;
}

void Methods::initDirectory() {
    // Declare variables
    std::string directory;

    // Ask user for directory
    std::cout << "Enter directory: ";
    std::cin >> directory;
    std::cout << std::endl;

    // Initialize directory
    std::cout << "Initializing directory... ";
    if(initDirectory(directory)) {
        std::cout << "Success." << std::endl;
    } else {
        std::cerr << "Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::listIndexes() {
    // Declare variables
    std::string directory;

    // Ask user for directory
    std::cout << "Enter directory: ";
    std::cin >> directory;
    std::cout << std::endl;

    // List indexes
    std::cout << "Listing indexes..." << std::endl;
    if(listIndexes(directory)) {
        std::cout << "Result: Success." << std::endl;
    } else {
        std::cerr << "Result: Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::listRequests() {
    // Declare variables
    std::string directory;

    // Ask user for directory
    std::cout << "Enter directory: ";
    std::cin >> directory;
    std::cout << std::endl;

    // List requests
    std::cout << "Listing requests..." << std::endl;
    if(listRequests(directory)) {
        std::cout << "Result: Success." << std::endl;
    } else {
        std::cerr << "Result: Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::sha1sum() {
    // Declare variables
    std::string path;

    // Ask user for file location
    std::cout << "Enter file location: ";
    std::cin >> path;
    std::cout << std::endl;

    // Calculate digest
    std::cout << "Calculating digest..." << std::endl;
    if(sha1sum(path)) {
        std::cout << "Result: Success." << std::endl;
    } else {
        std::cerr << "Result: Error." << std::endl;
    }
    std::cout << std::endl;
}

void Methods::showVersion() {
    std::cout << "Created: Aug 20, 2012" << std::endl;
    std::cout << "Updated: Sep 20, 2013" << std::endl;
    std::cout << "Version: 0.2" << std::endl;
    std::cout << std::endl;
    std::cout << "Press enter to continue." << std::endl;
}

void Methods::sync() {
    // Declare variables
	std::string local_root;
	std::string portable_root;

    // Ask user for local directory
	std::cout << "Enter local directory: ";
	std::cin >> local_root;
	std::cout << std::endl;

    // Ask user for portable directory
	std::cout << "Enter portable directory: ";
	std::cin >> portable_root;
	std::cout << std::endl;

    std::cout << "Syncing directories..." << std::endl;
	if(sync(local_root, portable_root)) {
        std::cout << "Result: Success." << std::endl;
	} else {
        std::cerr << "Result: Error." << std::endl;
	}
	std::cout << std::endl;
}
