#pragma once
#include "Token.hpp"
#include "WordCounter.hpp"
#include <fstream>
#include <unordered_set>
#include <vector>

class Tokenizer {
  public:
    [[nodiscard]] std::vector<Token> tokenize(std::fstream &) const;
  private:

    //privated methods
    [[nodiscard]] static bool   isStopWord(const std::string &);
    [[nodiscard]] static double scoreToken(const WordCounterToken &);
    [[nodiscard]] static bool   transform_word(std::string &); // returns false if the word is stopword

    //private variables
    static const std::unordered_set<std::string> STOP_WORDS;
    const std::string m_OutputDir;
};
