#include "Tokenizer.hpp"
#include <fstream>
#include <algorithm>

std::vector<std::string> Tokenizer::tokenize(std::fstream & file) const {
  std::vector<std::string> out;
  while(!file.eof()) {
    std::string word;
    file >> word;
    erase_if(word, [](unsigned char c){
        return !std::isalnum(static_cast<unsigned char>(c));
        });
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    out.push_back(word);
  }
  return out;
}
