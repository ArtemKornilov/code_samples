#ifndef TOKENIZER_H
#define TOKENIZER_H


class Tokenizer {
private:
	void MakeValidToken(std::string &);
	int WordCount;

public:
	Tokenizer() : WordCount(0) {};
	virtual ~Tokenizer() {};
	void Tokenize(std::map<std::string, int> &, std::ifstream &, int );
	int GetWordCount();

};



void Tokenizer::MakeValidToken(std::string &badToken) {

	std::size_t pos;

	pos = badToken.find_last_not_of(",.;:$%&[]()/\n\t|\\?!'\"*#<>");
	if (pos != std::string::npos)
		badToken.erase(pos + 1);                                      //cutting invalid chars at the end

	pos = badToken.find_first_not_of(",.;:$%&[]()/\n\t|\\?!'\"*#<>");
	if (pos != std::string::npos)
		badToken.erase(0, pos);										//cutting invalid chars at the start
};

void Tokenizer::Tokenize(std::map<std::string, int> &words, std::ifstream &inputFile, int lineCount) {

	std::string token;
	std::string line;

	for (int i = 1; i <= lineCount; i++) {

		std::getline(inputFile, line);
		std::istringstream tokens(line);

		while (tokens >> token) {


			MakeValidToken(token);
			
			words[token]++;
			WordCount++;
		}
	}
};

int Tokenizer::GetWordCount() {

	return WordCount;
}

#endif