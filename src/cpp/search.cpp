#include "search.h"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

void hello_world() {
    std::cout << "Hello world" << std::endl;
}

void tokenize_folder(const char* folderName) { // must be a const char in argumnet, bcs golang accept only C interface
    std::filesystem::path folder = folderName;
    Tokenizer T;
    for (const auto& entry: std::filesystem::directory_iterator(folder)) {
        if (std::filesystem::is_regular_file(entry)) {
            std::cout << "Tokenizing file: " << entry.path() << '\n';
            std::fstream file(entry.path());
            T.tokenize(file);
        }
    }
    std::cout.flush();
}
