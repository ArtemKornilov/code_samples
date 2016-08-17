#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <ctime>
#include <openssl/rand.h>
#include <openssl/des.h>
#include <boost/filesystem/operations.hpp>
#include "imagecoder.hpp"


using namespace boost::filesystem;

class Cipher {
public:
	
	Cipher() {};
	~Cipher() {};

	void groupUp(path &Directory) {

		//Grouping up
		if (is_directory(Directory)) {
			for (auto &it : directory_iterator(Directory)) {

				//Obtaing last write time info
				time_t time = last_write_time(it.path());
				struct tm *timeInfo = localtime(&time);

				//last write time since January 1
				int fileDay = timeInfo->tm_yday;
				std::string fileName = it.path().filename().string();

				if (it.path().extension() == ".jpg") { // or .png or .gif etc.

					fileType = IMAGE;
					groupes[std::make_pair(fileDay, fileType)].push_back(fileName);
				}

				else if (it.path().extension() == ".txt") {

					fileType = TEXT;
					groupes[std::make_pair(fileDay, fileType)].push_back(fileName);
				}

				else {
					std::cout << "File is neither an image nor a text" << std::endl;
				}

			}
		}
	}





	void encrypt(path &filesPlace, path &keysPlace) {

		if (!groupes.size()) {
			std::cout << "Files are not grouped" << std::endl;
			return;
		}

		for (auto it : groupes) {

			std::ostringstream dataToEncode;
			auto group = it.second;

			for (auto currentFile : group) {

				std::ifstream input(filesPlace.string() + currentFile, std::ios::binary);
				if (!input.is_open()) {
					std::cout << "Wrong directory: " << filesPlace.string();
					exit(0);
				}

				//Obtaining file data
				std::string buffer((
					std::istreambuf_iterator<char >(input)),
					(std::istreambuf_iterator<char >()));


				
				//Encode to hex if file is an image
				if (it.first.second == IMAGE) {

					std::string encoded_image;
					bn::encode_b16(buffer.begin(), buffer.end(), std::back_inserter(encoded_image));
					dataToEncode << currentFile << std::endl << encoded_image.size() << std::endl << encoded_image;
				}
				else {

					dataToEncode << currentFile << std::endl << buffer.size() << std::endl << buffer;
				}

				input.close();
			}

			//Size of files to decode
			int size = dataToEncode.tellp();

			DES_cblock key;
			DES_random_key(&key);

			DES_key_schedule key_schedule;
			DES_set_key_checked(&key, &key_schedule);

			//ncbc block magic
			size_t len = (size + 7) / 8 * 8;
			char *output = new char[len + 1];

			DES_cblock ivec;
			memset((char*)&ivec, 0, sizeof(ivec));

			DES_ncbc_encrypt((unsigned char *)&dataToEncode.str()[0], 
				             (unsigned char *)output, size, &key_schedule, &ivec, DES_ENCRYPT);


			std::string Day = std::to_string(it.first.first);
			std::string Type = it.first.second == TEXT ? "txt" : "image";
			std::string encodedFileName = Type + "_" + Day;

			std::ofstream outputFile(filesPlace.string() + encodedFileName, 
				                     std::ofstream::in | std::ofstream::trunc | std::ofstream::binary);

			std::ofstream outputKey(keysPlace.string() + encodedFileName + "_key", 
				                     std::ofstream::in | std::ofstream::trunc | std::ofstream::binary);
			

			if (!outputFile.is_open()) {
				std::cout << "Cannot create output file: " << encodedFileName;
				exit(0);
			}
			outputFile.write(output, len);

			

			if (!outputKey.is_open()) {
				std::cout << "Cannot create output key for file: " << encodedFileName;
				exit(0);
			}
			outputKey.write((char *)key, sizeof(key));

			outputFile.close();
			outputKey.close();

		}
	}

	void decrypt(path &filePlace, path &keyPlace) {
		
		std::ifstream fileToDecode(filePlace.string(), std::ios::binary);
		std::ifstream fileWithKey(keyPlace.string(), std::ios::binary);

		if (!fileToDecode.is_open()) {
			std::cout << "Cannot open file to decode: " << filePlace.filename().string();
			exit(0);
		}
		if (!fileWithKey.is_open()) {
			std::cout << "Cannot open file with key: " << keyPlace.filename().string();
			exit(0);
		}

		DES_cblock key;
		fileWithKey.read((char *)key, file_size(keyPlace));

		DES_key_schedule key_schedule;
		DES_set_key_checked(&key, &key_schedule);

		DES_cblock ivec;
		memset((char*)&ivec, 0, sizeof(ivec));

		char *inputData = new char[file_size(filePlace)];
		char *outputData = new char[file_size(filePlace)];
		memset(inputData, 0, file_size(filePlace));
		memset(outputData, 0, file_size(filePlace));
		fileToDecode.read(inputData, file_size(filePlace));

		DES_ncbc_encrypt((unsigned char *)inputData, (unsigned char *)outputData, 
			             file_size(filePlace), &key_schedule, &ivec, DES_DECRYPT);

		std::stringstream outputDataStream(outputData);
		std::string newFileName;
		std::string size;


		//Loop for file data
		while (std::getline(outputDataStream, newFileName)) {

			std::getline(outputDataStream, size);
			int fileSize = std::stoi(size);

			std::string newData;
			newData.resize(fileSize + 1);
			outputDataStream.read(&newData[0], fileSize);


			std::ofstream outputFile;

			//if on Linux
			if (filePlace.parent_path().string()[0] == '/') {
				outputFile.open(filePlace.parent_path().string() + "/" + newFileName,
				    			std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
			}
			else {
				outputFile.open(filePlace.parent_path().string() + "\\" + newFileName,
				             	std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
			}

			if (!outputFile.is_open()) {
				std::cout << "Cannot create new file: " << newFileName;
				exit(0);
			}


			//If it`s an image then decode back
			path p(newFileName);
			if (p.extension() != ".txt") {

				char *decoded_image = new char[fileSize + 1];
				bn::decode_b16(newData.begin(), newData.end(), decoded_image);

				outputFile.write(decoded_image, fileSize);
			}
			else {

				outputFile.write(&newData[0], fileSize);
			}

			outputFile.close();
		}


		fileToDecode.close();
		fileWithKey.close();

		delete[] inputData;
		delete[] outputData;
	}

private:
	std::map<std::pair<int, int>, 
		  std::list<std::string>> groupes;
	enum extensionType { TEXT, IMAGE };
	extensionType fileType;

};

int main(int argc, char **argv) {
	
	if (argc < 6 || argc > 6) {
		std::cout << "Usage: ./prog --encrypt --files_dir= --keys_dir=" << std::endl
			      << "or ./prog --decrypt --file_dir= --key-dir=" << std::endl;
		return 0;
	}

	Cipher cipher;

	if (!strcmp(argv[1], "--encrypt")) {

		path filesDir(argv[3]);
		path keysDir(argv[5]);

		cipher.groupUp(filesDir);
		cipher.encrypt(filesDir, keysDir);
	}

	else if (!strcmp(argv[1], "--decrypt")) {

		path fileDir(argv[3]);
		path keyDir(argv[5]);

		cipher.decrypt(fileDir, keyDir);
	}
	 
	else {
		std::cout << "Usage: ./prog --encrypt --files_dir= --keys_dir=" << std::endl
			<< "or ./prog --decrypt --file_dir= --key-dir=" << std::endl;
	}


	return 0;
}