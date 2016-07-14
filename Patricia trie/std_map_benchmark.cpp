#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <stdio.h>

int main(int argc, char* argv[]) {

    std::map<std::string, unsigned long long> test;
    std::map<std::string, unsigned long long>::iterator it;
    std::pair<std::map<std::string, unsigned long long>::iterator, bool> ret;

    std::string word;
    std::string bufferWord;

    int character;
    unsigned long long number;

    clock_t insertT = 0;
    clock_t searchT = 0;
    clock_t deleteT = 0;
    clock_t t;
    

    while ((character = std::cin.get()) != EOF){

        

        switch (character) {

        case '+':

            t = clock();

            std::cin >> std::ws;
            std::cin >> word;
            std::cin >> number;

            for (int i = 0; i < word.length(); ++i) {
                if (word[i] >= 'A' && word[i] <= 'Z') {
                    word[i] -= 'A' - 'a';
                }
            }
            ret = test.insert(std::pair<std::string, unsigned long long>(word, number));
            if (ret.second) {
                //std::cout << "OK" << std::endl;
            }
            else {
                //std::cout << "Exist" << std::endl;
            }

            insertT += clock() - t;
            break;

        case '-':

            t = clock();
            std::cin >> std::ws;
            std::cin >> word;

            for (int i = 0; i < word.length(); ++i) {
                if (word[i] >= 'A' && word[i] <= 'Z') {
                    word[i] -= 'A' - 'a';
                }
            }

            it = test.find(word);

            if (it != test.end()) {
                test.erase(it);
                //std::cout << "OK" << std::endl;
            }
            else {
                //std::cout << "NoSuchWord" << std::endl;
            }
            deleteT += clock() - t;
            break;

        case '\n':
            break;

        default:

            t = clock();

            word += character;
            std::cin >> std::ws;
            std::cin >> bufferWord;
            word += bufferWord;

            for (int i = 0; i < word.length(); ++i) {
                if (word[i] >= 'A' && word[i] <= 'Z') {
                    word[i] -= 'A' - 'a';
                }
            }

            it = test.find(word);

            if (it != test.end()) {
                //std::cout << "OK: " << it->second << std::endl;
            }
            else {
                //std::cout << "NoSuchWord" << std::endl;
            }

            searchT += clock() - t;
            break;
        }

        word.clear();
        std::cin >> std::ws;
    }

    std::cout << "Search time: " << ((float)searchT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Delete time: " << ((float)deleteT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Insert time: " << ((float)insertT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Time at all: " << ((float)searchT)/CLOCKS_PER_SEC + 
                                    ((float)deleteT)/CLOCKS_PER_SEC +
                                    ((float)insertT)/CLOCKS_PER_SEC  << " seconds" << std::endl;


    return 0;
}
