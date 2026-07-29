#include "Tokenizer.hpp"
#include "SectionsEnum.hpp"
#include "Token.hpp"
#include "WordCounter.hpp"
#include <cassert>
#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <unordered_map>


const std::unordered_set<std::string> Tokenizer::STOP_WORDS = {
    "a", "about", "above", "after", "again", "against", "all", "am",
    "an", "and", "any", "are", "as", "at",

    "be", "because", "been", "before", "being", "below", "between",
    "both", "but", "by",

    "can", "could",

    "did", "do", "does", "doing", "down", "during",

    "each",

    "few", "for", "from", "further",

    "had", "has", "have", "having", "he", "her", "here", "hers",
    "herself", "him", "himself", "his", "how",

    "i", "if", "in", "into", "is", "it", "its", "itself",

    "just",

    "me", "more", "most", "my", "myself",

    "no", "nor", "not", "now",

    "of", "off", "on", "once", "only", "or", "other", "our",
    "ours", "ourselves", "out", "over", "own",

    "same", "she", "should", "so", "some", "such",

    "than", "that", "the", "their", "theirs", "them", "themselves",
    "then", "there", "these", "they", "this", "those", "through",
    "to", "too",

    "under", "until", "up",

    "very",

    "was", "we", "were", "what", "when", "where", "which", "while",
    "who", "whom", "why", "will", "with",

    "you", "your", "yours", "yourself", "yourselves"
};

double Tokenizer::scoreToken(const WordCounterToken & token, size_t wordsInDocument) {
    double score = 0.;
    for (Section s: Sections) {
        score += static_cast<double>(token.getSectionCount(s)) * sectionWeight(s);
    }
    return (score) / static_cast<double>(wordsInDocument);
}

bool Tokenizer::isStopWord(const std::string & str) {
  return STOP_WORDS.contains(str);
}

bool Tokenizer::transform_word(std::string & word) {
      //remove non alphanumeric characters
      erase_if(word, [](unsigned char c){
          if (std::isalnum(static_cast<unsigned char>(c))) 
              return false;
          return true;
      });

      //transform to lowercase
      std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {
          return std::tolower(c);
      });

      if (word=="") return false;
      //if it's not stopword add it into output
      if (isStopWord(word))
          return false;
      return true;
}

std::vector<Token> Tokenizer::tokenize(std::ifstream & file) const {
    std::vector<Token> output;
    std::unordered_map<std::string, WordCounterToken> wordCounter;
    std::string line;
    size_t numberOfWords = 0;

    while(std::getline(file, line)) {
        std::stringstream stream(line);
        std::string secStr;
        stream >> secStr;
        secStr.pop_back();
        if (secStr.empty()) continue;
        Section section = getSection(secStr);

        //std::cout << "Parsing section:" << secStr << std::endl; //Only for debuging
        //skip parsing URLS, Language and undefined tags
        if (section == Section::URL  || 
            section == Section::Lang ||
            section == Section::Wrong) continue;

        while (!stream.eof()) {
            std::string word;
            stream >> word;
            //std::cout << "Word: "<< word << std::endl; //Only for debuging
            if (!transform_word(word)) continue;
            numberOfWords++;
            if (wordCounter.contains(word)) {
                WordCounterToken token = wordCounter.at(word);
                token.incSection(section);
            } else {
                WordCounterToken token;
                token.incSection(section);
                wordCounter[word] = token;
            }
        }
    }

    //insert into output vector
    for (auto &[w, tok]: wordCounter) {
        output.push_back(Token(w, scoreToken(tok, numberOfWords)));
    }

    //sort by how many times occured
    std::sort(output.begin(), output.end(), [](const Token & t1, const Token & t2) {
        return t1.getScore() > t2.getScore();
    });

    return output;
}
