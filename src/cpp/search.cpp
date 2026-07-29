#include "search.h"
//#include "Tokenizer.hpp"
#include <iostream>
#include <filesystem>

void hello_world() {
    std::cout << "Hello world" << std::endl;
}

void tokenize_folder(const char* folderName) {
    std::filesystem::path folder = folderName;

    for (const auto& entry: std::filesystem::directory_iterator(folder)) {
        if (std::filesystem::is_regular_file(entry)) {
            std::cout << "Tokenizing file: " << entry.path() << '\n';
        }
    }
    std::cout.flush();
}
