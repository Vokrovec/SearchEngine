#pragma once
#include <fstream>
#include <vector>

class Tokenizer {
  public:
    Tokenizer() {};
    std::vector<std::string> tokenize(std::fstream &);
};
