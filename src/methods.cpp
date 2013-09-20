//============================================================================
// Name        : methods.cpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

// Load headers
#include "methods.hpp"

//
// Private Methods
//

void Methods::createRequest(std::string file_hash, std::string root_directory) {
	// Declare variables
	bool alreadyRequested = false;
	bool requestAdded = false;
	request_registry::Registry aRequestRegistry;
	std::string rr_file = root_directory + "/request_registry.proto";

	// Load request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry." << std::endl;
		return;
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
		std::cout << file_hash + ": already requested" << std::endl;
	}

	if(requestAdded) {
        // Update request registry
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!aRequestRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Failed to write request registry." << std::endl;
	    }
	    output.close();
	}
}

void Methods::exportFiles(std::string local_root, std::string portable_root) {
	// Declare variables
	bool updateRequestRegistry = false;
	Crypto aCrypto;
	file_registry::File *aFile;
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;
	request_registry::Request *aRequest;
	std::string actual_hash;
	std::string fr_file = local_root + "/file_registry.proto";
	std::string rr_file = portable_root + "/request_registry.proto";
	std::string file_path;

	std::cout << "Exporting files..." << std::endl;

	// Load file registry
	std::fstream fr_input(fr_file.c_str(), std::ios::in | std::ios::binary);
    if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Failed to parse file registry" << std::endl;
		return;
	}
	fr_input.close();

	// Load request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry" << std::endl;\
		return;
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
						std::cout << "Error: Invalid hash." << std::endl;
					}
					break;
				}
			}
		}
	}

	if(updateRequestRegistry) {
        // Update request registry
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!aRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Failed to write request registry." << std::endl;
		}
		output.close();

		updateFileRegistry(portable_root);
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::importFiles(std::string local_root, std::string portable_root) {
    // Declare variables
    bool updateRequestRegistry = false;
    Crypto aCrypto;
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;
	request_registry::Request *aRequest;
	std::string file_path;
    std::string fr_file = portable_root + "/file_registry.proto";
	std::string rr_file = local_root + "/request_registry.proto";

	std::cout << "Importing files..." << std::endl;

    // Load file registry
	std::fstream fr_input(fr_file.c_str(), std::ios::in | std::ios::binary);
    if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Failed to parse file registry" << std::endl;
		return;
	}
	fr_input.close();

	// Load request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);
    if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry" << std::endl;\
		return;
	}
    rr_input.close();

	for(int i = 0; i< aRequestRegistry.request_size(); i++) {
		aRequest = aRequestRegistry.mutable_request(i);

		if(aRequest->active()) {
			for(int j = 0; j < aFileRegistry.file_size(); j++) {
				const file_registry::File& aFile = aFileRegistry.file(j);
				if(aFile.hash() == aRequest->hash()) {
					std::string actual_hash = aCrypto.sha1sum(portable_root + aFile.path());

					if(aFile.hash() == actual_hash) {
						boost::filesystem::copy_file(
							portable_root + aFile.path(),
							local_root + "/shared/" + aFile.name(),
							boost::filesystem::copy_option::overwrite_if_exists
						);
						aRequest->set_active(false);
						updateRequestRegistry = true;
					} else {
						std::cout << "Error: Invalid hash." << std::endl;
					}
				}
			}
		}
	}

	if(updateRequestRegistry) {
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!aRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Failed to write request registry." << std::endl;
		}
		output.close();
		updateFileRegistry(local_root);
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::exportRequests(std::string local_root_dir, std::string portable_root_dir) {
	bool requestFound = false;
	bool updatePortable = false;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file = local_root_dir + "/request_registry.proto";
	std::string portable_file = portable_root_dir + "/request_registry.proto";

	std::cout << "Exporting requests..." << std::endl;

	// Load local request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Failed to parse local request registry." << std::endl;
		return;
	}
	local_input.close();

	// Load portable request registry
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
	if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Failed to parse portable request registry." << std::endl;
		return;
	}
	portable_input.close();

	for(int i = 0; i< localRequestRegistry.request_size(); i++) {
		request_registry::Request *aLocalRequest = localRequestRegistry.mutable_request(i);
		requestFound = false;

		std::cout << aLocalRequest->hash() << aLocalRequest->active() << std::endl;

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
	    	std::cerr << "Failed to write request registry." << std::endl;
	    }
	    output.close();
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::importRequests(std::string local_root_dir, std::string portable_root_dir) {
	bool requestFound = false;
	bool updateLocal = false;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file = local_root_dir + "/request_registry.proto";
	std::string portable_file = portable_root_dir + "/request_registry.proto";

	std::cout << "Importing requests... " << std::endl;

	// Load local request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Failed to parse local request registry." << std::endl;
		return;
	}
	local_input.close();

	// Load portable request registry
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);
    if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Failed to parse portable request registry." << std::endl;
		return;
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
			std::cerr << "Failed to write request registry." << std::endl;
		}
		output.close();
	}
	std::cout << "Result: Success." << std::endl;
}

void Methods::initDirectory(std::string root_directory) {
    // Declare variables
    FILE *pFile;
	std::string fr_file = root_directory + "/file_registry.proto";
	std::string rr_file = root_directory + "/request_registry.proto";
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

    // Check for share folder
	if(!boost::filesystem::is_directory(share_folder)) {
        // Create share folder
		boost::filesystem::create_directory(share_folder);
	}
}

void Methods::sync(std::string local_root, std::string portable_root) {
    std::cout << "Syncing directories..." << std::endl;

    // Initialize directories
    initDirectory(local_root);
    initDirectory(portable_root);

    // Update request registries
    updateRequestRegistry(local_root);
    updateRequestRegistry(portable_root);

    // Update file registries
    updateFileRegistry(local_root);
    updateFileRegistry(portable_root);

    // Sync files
    exportFiles(local_root, portable_root);
    importFiles(local_root, portable_root);

    // Sync requests
    exportRequests(local_root, portable_root);
    importRequests(local_root, portable_root);

    std::cout << "Result: Success." << std::endl;
}

void Methods::updateFileRegistry(std::string root_directory) {
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
		std::cerr << "Failed to parse file registry." << std::endl;
		return;
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
			std::cout << fileHash + ": already discovered" << std::endl;
		}
	}

	if(fileDiscovered) {
		std::fstream output(registry_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!aFileRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Failed to write file registry." << std::endl;
	    }
	    output.close();
	}
}

void Methods::updateRequestRegistry(std::string root_directory) {
	request_registry::Registry newRequestRegistry;
	request_registry::Registry oldRequestRegistry;
    std::string rr_file = root_directory + "/request_registry.proto";

    // Load request registry
	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);
	if(!oldRequestRegistry.ParseFromIstream(&input)) {
		std::cerr << "Unable to parse request registry" << std::endl;
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
    	std::cerr << "Failed to write request registry." << std::endl;
    }
    output.close();
}

//
// Public Methods
//

void Methods::createRequest() {
    // Declare variables
    std::string file_hash, root_directory;

    std::cout << "Enter directory: ";
    std::cin >> root_directory;

    std::cout << "Enter file hash: ";
    std::cin >> file_hash;

    createRequest(file_hash, root_directory);
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
    std::string root_directory;
    std::cout << "Enter directory: ";
    std::cin >> root_directory;
    initDirectory(root_directory);
    std::cout << "Result: Success." << std::endl;
    std::cout << std::endl;
}

void Methods::listRequests() {
	std::string root_directory;
	std::string rr_file;
	request_registry::Registry aRequestRegistry;

	std::cout << "Enter directory: ";
	std::cin >> root_directory;

	std::cout << "Listing requests..." << std::endl;

	rr_file = root_directory + "/request_registry.proto";

	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);

	if(!aRequestRegistry.ParseFromIstream(&input)) {
		std::cerr << "Unable to parse request registry" << std::endl;
	}

	for(int i = 0; i < aRequestRegistry.request_size(); i++) {
		const request_registry::Request &aRequest = aRequestRegistry.request(i);
		std::cout << "Request - " << aRequest.hash() << " - " << aRequest.active() << " - " << aRequest.timeout() << std::endl;
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::sha1sum() {
    std::string hash, path;

    std::cout << "Enter path: ";
    std::cin >> path;

    std::cout << std::endl;
    std::cout << "Calculating digest..." << std::endl;

    Crypto aCrypto;
    hash = aCrypto.sha1sum(path);
    std::cout << hash << std::endl;
    std::cout << std::endl;
    hash.clear();
}

void Methods::showVersion() {
    std::cout << "Created: Aug 20, 2012" << std::endl;
    std::cout << "Updated: Sep 17, 2013" << std::endl;
    std::cout << "Version: 0.2" << std::endl;
    std::cout << std::endl;
    std::cout << "Press enter to continue." << std::endl;
}

void Methods::sync() {
	std::string local_root;
	std::string portable_root;

	std::cout << "Enter local directory: ";
	std::cin >> local_root;

	std::cout << "Enter portable directory: ";
	std::cin >> portable_root;

	sync(local_root, portable_root);
}
