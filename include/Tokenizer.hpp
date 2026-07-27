#pragma once
#include <fstream>
#include <vector>

class Tokenizer {
  public:
    std::vector<std::string> tokenize(std::fstream &) const;
};
