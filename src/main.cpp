#include <iostream>
#include <fstream>
#include <vector>
#include "Tokenizer.hpp"

int main (int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong number of arguments!\n";
    std::cerr << "Expected name of search file!" << std::endl;
    return 1;
  }
  std::string filename = argv[1];
  std::fstream file;
  file.open(filename);
  if (!file.is_open()) {
    std::cerr << "File cannot be opened!" << std::endl;
    return 1;
  }
  Tokenizer tokenizer;
  std::vector<std::string> tokens = tokenizer.tokenize(file);
  for (std::string & s: tokens)
    std::cout << s << ' ';
  std::cout << std::endl;
  return 0;
}
