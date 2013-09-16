//============================================================================
// Name        : methods.cpp
// Author      : Justin Holz
// Version     : 0.2
// Copyright   : Creative Commons Attribution–ShareAlike License | http://freedomdefined.org/Licenses/CC-BY-SA
// Description : Project Sparrow | Offline File-Sharing Program
//============================================================================

#include "methods.hpp"

//
// Private Methods
//

void Methods::exportFiles(std::string local_root, std::string portable_root) {
	// Declare local variables
	bool updateRequestRegistry;
	file_registry::File *aFile;
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;
	request_registry::Request *aRequest;
	std::string fr_file, rr_file;
	std::string file_path;

	std::cout << "Exporting files..." << std::endl;

	// Set Local Variables
	fr_file = local_root + "/file_registry.proto";
	rr_file = portable_root + "/request_registry.proto";
	updateRequestRegistry = false;

	// Load Protocol Buffers
	std::fstream fr_input(fr_file.c_str(), std::ios::in | std::ios::binary);
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);

	if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Failed to parse file registry" << std::endl;
		return;
	}

	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry" << std::endl;\
		return;
	}

	for(int i = 0; i< aRequestRegistry.request_size(); i++) {
		aRequest = aRequestRegistry.mutable_request(i);

		// Check if request is active
		if(aRequest->active()) {
			for(int j = 0; j < aFileRegistry.file_size(); j++) {
				aFile = aFileRegistry.mutable_file(j);
				if(aFile->hash() == aRequest->hash()) {
					std::string actual_path = local_root + "/" + aFile->path();
					std::string actual_hash;
					CryptoPP::SHA1 hashFunction;

					CryptoPP::FileSource(
						actual_path.c_str(),
						true,
						new CryptoPP::HashFilter(
							hashFunction,
							new CryptoPP::HexEncoder(
								new CryptoPP::StringSink(
									actual_hash
								),
								false
							)
						)
					);

					if(aFile->hash() == actual_hash) {
						boost::filesystem::copy_file(
							local_root + aFile->path(),
							portable_root + "/shared/" + aFile->name(),
							boost::filesystem::copy_option::overwrite_if_exists
						);
						aRequest->set_active(false);
						updateRequestRegistry = true;
					} else {
						std::cout << "Invalid hash" << std::endl;
					}
					break;
				}
			}
		}
	}

	if(updateRequestRegistry) {
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!aRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Failed to write request registry." << std::endl;
		}
		updateFileRegistry(portable_root);
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::importFiles(std::string local_root, std::string portable_root) {
	file_registry::Registry aFileRegistry;
	request_registry::Registry aRequestRegistry;

	std::cout << "Importing files..." << std::endl;

	std::string fr_file = portable_root + "/file_registry.proto";
	std::string rr_file = local_root + "/request_registry.proto";

	std::fstream fr_input(fr_file.c_str(), std::ios::in | std::ios::binary);
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);

	if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Failed to parse file registry" << std::endl;
		return;
	}

	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry" << std::endl;\
		return;
	}

	std::string file_path;
	bool updateRequestRegistry = false;

	for(int i = 0; i< aRequestRegistry.request_size(); i++) {
		request_registry::Request *aRequest = aRequestRegistry.mutable_request(i);

		if(aRequest->active()) {
			for(int j = 0; j < aFileRegistry.file_size(); j++) {
				const file_registry::File& aFile = aFileRegistry.file(j);
				if(aFile.hash() == aRequest->hash()) {
					std::string actual_path = portable_root + aFile.path();

					std::string actual_hash;
					CryptoPP::SHA1 hashFunction;

					CryptoPP::FileSource(
						actual_path.c_str(),
						true,
						new CryptoPP::HashFilter(
							hashFunction,
							new CryptoPP::HexEncoder(
								new CryptoPP::StringSink(
									actual_hash
								),
								false
							)
						)
					);

					if(aFile.hash() == actual_hash) {
						boost::filesystem::copy_file(
							portable_root + aFile.path(),
							local_root + "/shared/" + aFile.name(),
							boost::filesystem::copy_option::overwrite_if_exists
						);
						aRequest->set_active(false);
						updateRequestRegistry = true;
					} else {
						std::cout << "Invalid hash" << std::endl;
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
		updateFileRegistry(local_root);
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::exportRequests(std::string local_root_dir, std::string portable_root_dir) {
	bool requestFound, updatePortable;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file, portable_file;

	std::cout << "Exporting requests..." << std::endl;

	local_file = local_root_dir + "/request_registry.proto";
	portable_file = portable_root_dir + "/request_registry.proto";
	requestFound = false;
	updatePortable = false;

	// Read the existing request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);

	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Failed to parse local request registry." << std::endl;
		return;
	}

	if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Failed to parse portable request registry." << std::endl;
		return;
	}

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
		std::fstream output(portable_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!portableRequestRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Failed to write request registry." << std::endl;
	    }
	}

	std::cout << "Result: Success." << std::endl;
}

void Methods::importRequests(std::string local_root_dir, std::string portable_root_dir) {
	bool requestFound, updateLocal;
	request_registry::Registry localRequestRegistry, portableRequestRegistry;
	std::string local_file, portable_file;

	std::cout << "Importing requests... " << std::endl;

	local_file = local_root_dir + "/request_registry.proto";
	portable_file = portable_root_dir + "/request_registry.proto";
	requestFound = false;
	updateLocal = false;

	// Read the existing request registry
	std::fstream local_input(local_file.c_str(), std::ios::in | std::ios::binary);
	std::fstream portable_input(portable_file.c_str(), std::ios::in | std::ios::binary);

	if(!localRequestRegistry.ParseFromIstream(&local_input)) {
		std::cerr << "Failed to parse local request registry." << std::endl;
		return;
	}

	if(!portableRequestRegistry.ParseFromIstream(&portable_input)) {
		std::cerr << "Failed to parse portable request registry." << std::endl;
		return;
	}

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
		std::fstream output(local_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		if(!localRequestRegistry.SerializePartialToOstream(&output)) {
			std::cerr << "Failed to write request registry." << std::endl;
		}
	}
	std::cout << "Result: Success." << std::endl;
}

void Methods::initDirectory(std::string root_directory) {
	std::string fr_file = root_directory + "/file_registry.proto";
	std::string rr_file = root_directory + "/request_registry.proto";
	std::string share_folder = root_directory + "/shared";

	FILE *pFile;

	if(!boost::filesystem::exists(fr_file)) {
		pFile = fopen(fr_file.c_str(), "w");
		if(pFile != NULL) {
			fclose(pFile);
		}
	}

	if(!boost::filesystem::exists(rr_file)) {
		pFile = fopen(rr_file.c_str(), "w");
		if(pFile != NULL) {
			fclose(pFile);
		}
	}

	if(!boost::filesystem::is_directory(share_folder)) {
		boost::filesystem::create_directory(share_folder);
	}
}

void Methods::updateFileRegistry(std::string root_directory) {
	bool alreadyDiscovered, fileDiscovered;
	file_registry::Registry aFileRegistry;
	std::string registry_file;
	std::string share_directory;

	registry_file = root_directory + "/file_registry.proto";
	share_directory = root_directory + "/shared";

	boost::filesystem::path share_path(share_directory);
	boost::filesystem::directory_iterator iter(share_path), end;

	// Read the existing file registry
	std::fstream fr_input(registry_file.c_str(), std::ios::in | std::ios::binary);

	if(!aFileRegistry.ParseFromIstream(&fr_input)) {
		std::cerr << "Failed to parse file registry." << std::endl;
		return;
	}

	// Lookup each file in shared directory
	for(;iter != end; ++iter) {
		alreadyDiscovered = false;
		std::string fileName = iter->path().filename().string();
		std::string filePath = "/shared/" + fileName;
		std::string fileHash;
		CryptoPP::SHA1 hashFunction;

		std::string relative_path = share_directory + "/" + fileName;
		CryptoPP::FileSource(
			relative_path.c_str(),
			true,
			new CryptoPP::HashFilter(
				hashFunction,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(
						fileHash
					),
					false
				)
			)
		);

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
	}
}

void Methods::updateRequestRegistry(std::string root_directory) {
	request_registry::Registry newRequestRegistry;
	request_registry::Registry oldRequestRegistry;

	std::string rr_file = root_directory + "/request_registry.proto";

	std::fstream input(rr_file.c_str(), std::ios::in | std::ios::binary);

	if(!oldRequestRegistry.ParseFromIstream(&input)) {
		std::cerr << "Unable to parse request registry" << std::endl;
	}

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

	std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!newRequestRegistry.SerializeToOstream(&output)) {
    	std::cerr << "Failed to write request registry." << std::endl;
    }

	//Load Request Registry
	//Loop through Request Registry
	//Check timeout of requests
	//If timeout < now delete request and file
}

//
// Public Methods
//

void Methods::createRequest() {
	// Allow user to add request
	bool alreadyRequested = false;
	bool requestAdded = false;
	request_registry::Registry aRequestRegistry;

	std::cout << "Enter directory: ";
	std::string root_directory;
	std::cin >> root_directory;

	std::string rr_file = root_directory + "/request_registry.proto";

	// Read the existing request registry
	std::fstream rr_input(rr_file.c_str(), std::ios::in | std::ios::binary);

	if(!aRequestRegistry.ParseFromIstream(&rr_input)) {
		std::cerr << "Failed to parse request registry." << std::endl;
		return;
	}

	std::cout << "Add request: ";
	std::string file_hash;
	std::cin >> file_hash;

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
		std::fstream output(rr_file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	    if (!aRequestRegistry.SerializeToOstream(&output)) {
	    	std::cerr << "Failed to write request registry." << std::endl;
	    }
	}
}

void Methods::help() {
	std::cout << std::endl;
	std::cout << "Example of use:" << std::endl;
	std::cout << "1. Let's assume that a friend of a friend has a file you want." << std::endl;
	std::cout << "2. Get the SHA1 hash of that file." << std::endl;
	std::cout << "3. Execute createRequest and enter the SHA1 hash." << std::endl;
	std::cout << "4. syncDirectories with a portable device (ie MP3 player, USB Drive, etc)." << std::endl;
	std::cout << "5. Take the device to friends house and syncDirectories with their computer." << std::endl;
	std::cout << "6. Tell them to do the same with their friend and check back later." << std::endl;
	std::cout << "7. Return with your device and execute syncDirectories." << std::endl;
	std::cout << "8. Return home and syncDirectories with your computer." << std::endl;
	std::cout << "9. Voila! You have the file." << std::endl;
	std::cout << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "createRequest - Request file by SHA1 hash" << std::endl;
	std::cout << "listRequests - List file requests in registry" << std::endl;
	std::cout << "syncDirectories - Sync files and requests between local and portable directories" << std::endl;
	std::cout << std::endl;
	std::cout << "Press enter to continue." << std::endl;
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

void Methods::syncDirectories() {
	std::string local_root;
	std::string portable_root;

	std::cout << "Enter local directory: ";
	std::cin >> local_root;

	std::cout << "Enter portable directory: ";
	std::cin >> portable_root;

	initDirectory(local_root);
	initDirectory(portable_root);

	updateRequestRegistry(local_root);
	updateRequestRegistry(portable_root);

	updateFileRegistry(local_root);
	updateFileRegistry(portable_root);

	exportFiles(local_root, portable_root);
	importFiles(local_root, portable_root);

	exportRequests(local_root, portable_root);
	importRequests(local_root, portable_root);
}
