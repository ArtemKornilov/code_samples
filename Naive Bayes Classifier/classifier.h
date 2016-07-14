#ifndef CLASSIFIER_H
#define CLASSIFIER_H 

#include <map>

class Classifier {
private:
	int AllThreadCount;
	struct StatFile {
		std::map<std::string, int> Words;
		int ThreadCount;
		int WordCount;
	};


public:
	Classifier() : AllThreadCount(0) {};
	virtual ~Classifier() {};
	void HandleStatFile(std::ifstream &);
	double GetCurrentProbability(StatFile &, std::map<std::string, int> &);

	std::list<StatFile> StatFiles;
};


void Classifier::HandleStatFile(std::ifstream &file) {

	StatFile currentFile;
	currentFile.Words.clear();

	std::string string;
	int count;


	file >> currentFile.ThreadCount >> currentFile.WordCount;
	AllThreadCount += currentFile.ThreadCount;

	while (file >> string >> count) {
		currentFile.Words.insert(std::make_pair(string, count));
	}

	StatFiles.push_back(currentFile);
}

double Classifier::GetCurrentProbability(StatFile &currentFile, std::map<std::string, int> &wordsForClassify) {

	//probability that the word belongs to current class
	double probability = 0;
	int vocabrurySize = currentFile.Words.size();

	for (auto it : wordsForClassify) {
		auto node = currentFile.Words.find(it.first);
		int occurensesCount = node == currentFile.Words.end() ? 1 : node->second + 1;
		probability += std::log((occurensesCount) / (double)(currentFile.WordCount + currentFile.Words.size()));
	}


	//probabilty that the thread belongs to current class
	probability += std::log(currentFile.ThreadCount / (double)AllThreadCount);

	return probability;
}

#endif