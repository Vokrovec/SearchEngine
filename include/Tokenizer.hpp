#pragma once
#include <fstream>
#include <unordered_set>
#include <vector>

class Tokenizer {
  public:
    std::vector<std::string> tokenize(std::fstream &) const;
  private:
    static const std::unordered_set<std::string> STOP_WORDS;
    static bool isStopWord(const std::string &);
};
