#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <list>
#include <iostream>
#include <sstream>
#include <cstring>
#include "classifier.h"
#include "tokenizer.h"

static const int MODE = 1;
static const int ARGS_COUNT_MIN = 7;
static const int FIRST_FILE_PLACE = 3;
static const int GAP = 2;

void classify(int argc, char **argv) {
	
	if (argc < ARGS_COUNT_MIN || strcmp(argv[MODE], "classify") != 0) {
		std::cout << "Usage: ./prog classify --stats <stats file> --stats <stats file> [--stats <stats file> ...]  "
			"--input <input file> --output <output file>" << std::endl;
	}

	Classifier classifier;
	std::ifstream inputFile, statFile;
	std::ofstream outputFile;

	for (int i = FIRST_FILE_PLACE; i < argc; i += GAP) {

		if (strcmp(argv[i - 1], "--input") == 0) {

			inputFile.open(argv[i], std::ifstream::in);
			if (!inputFile.good()) {
				std::cout << "Error open file " << argv[i] << std::endl;
			}
		}
		else if (strcmp(argv[i - 1], "--output") == 0) {

			outputFile.open(argv[i], std::ifstream::trunc);
			if (!inputFile.good()) {
				std::cout << "Error open file " << argv[i] << std::endl;
			}
		}
		else if (strcmp(argv[i - 1], "--stats") == 0) {

			std::ifstream statFile(argv[i], std::ifstream::in);
			if (!statFile.good()) {
				std::cout << "Error open file " << argv[i] << std::endl;
			}
			classifier.HandleStatFile(statFile);
			statFile.close();
		}
		else {

			std::cout << "Usage: ./prog classify --stats <stats file> --stats <stats file> [--stats <stats file> ...]  "
				"--input <input file> --output <output file>" << std::endl;
		}
	}

	if (!inputFile.is_open() || !outputFile.is_open() || classifier.StatFiles.empty()) {
		std::cout << "Usage: ./prog classify --stats <stats file> --stats <stats file> [--stats <stats file> ...]  "
			"--input <input file> --output <output file>" << std::endl;
	}

	Tokenizer tokenizer;
	std::string line;
	std::map<std::string, int> words;
									

	//Answer contain probability and class
	std::pair<double, int> answer(0.0, 0);
	int lineCount;
	int marker;

	while (std::getline(inputFile, line)) {

		std::istringstream markerLine(line);
		markerLine >> marker >> lineCount;


		tokenizer.Tokenize(words, inputFile, lineCount);
		//if the thread has alredy been read
		int number = 1;
		//loop for all stat files
		for (auto file : classifier.StatFiles) { 
			
			double probability = classifier.GetCurrentProbability(file, words);

			if (answer.first == 0.0) {
				answer.first = probability;
				answer.second = number;
			}


			if (probability > answer.first) {
				answer.first = probability;
				answer.second = number;
			}

			number++;
		}


		words.clear();
		outputFile << answer.second << std::endl;
		answer.first = 0.0;
		answer.second = 0;
	}


	inputFile.close();
	outputFile.close();
}







static const int ARGS_COUNT = 6;
void learn(int argc, char **argv) {

	std::ifstream inputFile;
	std::ofstream outputFile;

	if (argc != ARGS_COUNT || strcmp(argv[MODE], "learn") != 0) {

		std::cout << "Usage: ./prog learn --input <input file> --output <output file>" << std::endl;
	}
	
	for (int i = FIRST_FILE_PLACE; i < argc; i += GAP) {

		if (strcmp(argv[i - 1], "--input") == 0) {

			inputFile.open(argv[i], std::ifstream::in);
			if (!inputFile.good()) {
				std::cout << "Error open file " << argv[i] << std::endl;
			}
		}
		else if (strcmp(argv[i - 1], "--output") == 0) {

			outputFile.open(argv[i], std::ifstream::trunc);
			if (!inputFile.good()) {
				std::cout << "Error open file " << argv[i] << std::endl;
			}
		}
		else {
			std::cout << "Usage: ./prog learn --input <input file> --output <output file>" << std::endl;
		}
	}

	if (!inputFile.is_open() || !outputFile.is_open()) {
		std::cout << "Usage: ./prog learn --input <input file> --output <output file>" << std::endl;
	}


	Tokenizer tokenizer;
	std::map<std::string, int> words;

	std::string line;
	int threadCount = 0;
	int lineCount;
	int marker;

	while (std::getline(inputFile, line)) {

		std::istringstream markerLine(line);
		markerLine >> marker >> lineCount;
		threadCount++;
		tokenizer.Tokenize(words, inputFile, lineCount);
	}

	outputFile << threadCount << std::endl;
	outputFile << tokenizer.GetWordCount() << std::endl;
	for (auto it : words) {
		outputFile << it.first << std::endl << it.second << std::endl;
	}


	inputFile.close();
	outputFile.close();
}





int main(int argc, char **argv) {

	if (strcmp(argv[MODE], "learn") == 0) {
		learn(argc, argv);
	}
	else if (strcmp(argv[MODE], "classify") == 0) {
		classify(argc, argv);
	}
	else {
		std::cout << '\"' << argv[MODE] << '\"' << " must be either \"learn\" or \"classify\"" << std::endl;
	}

	return 0;
}