#include "indexer/Tokenizer.hpp"
#include "common/Token.hpp"
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

void tokenize_folder(const std::string & inputFolderName, const std::string & outputFolderName) { // must be a const char in argumnet, bcs golang accept only C interface
    std::filesystem::path folder = inputFolderName;
    Tokenizer T;
    for (const auto& entry: std::filesystem::directory_iterator(folder)) {
        if (std::filesystem::is_regular_file(entry)) {
            std::cout << "Tokenizing inputFile: " << entry.path() << '\n';
            std::string filename = entry.path().filename();

            //open a inputFile
            std::ifstream inputFile(entry.path());
            if (!inputFile.is_open()) {
                std::cerr << "Can not open inputFile: " << entry.path() << std::endl;
                continue;
            }
            //tokenize
            std::vector<Token> tokens = T.tokenize(inputFile);
            std::string outputFileName = static_cast<std::string>(outputFolderName) + "/" + filename;
            std::ofstream outputFile(outputFileName);
            if (!outputFile.is_open()) {
                std::cerr << "Can not open outputFile: " << outputFileName << std::endl;
                continue;
            }
            //TODO: write to output file
            //std::cout << "Size of tokens: " << tokens.size() << std::endl; //DEBUG
            for (Token & t: tokens) {
                outputFile << t.getWord() << ':' << t.getScore() << ';';
                //std::cout << t.getWord() << ':' << t.getScore() << std::endl; //DEBUG
            }
            outputFile.flush();
        }
    }
    std::cout.flush();
}

int main() {
    tokenize_folder("./urls", "./tokenized");
}
