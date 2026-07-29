#pragma once
#include "Token.hpp"
#include <fstream>
#include <unordered_set>
#include <vector>
//TODO: make getScore method

class Tokenizer {
  public:
    std::vector<Token> tokenize(std::fstream &) const;
  private:

    //privated methods
    static bool isStopWord(const std::string &);

    //private variables
    static const std::unordered_set<std::string> STOP_WORDS;
    const std::string m_OutputDir;
};
