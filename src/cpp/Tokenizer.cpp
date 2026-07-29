#include "Tokenizer.hpp"
#include "SectionsEnum.hpp"
#include "Token.hpp"
#include "WordCounter.hpp"
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

double Tokenizer::scoreToken(const WordCounterToken & token) {
  return 1;// TODO: make this propper method
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

      //if it's not stopword add it into output
      if (!isStopWord(word))
          return false;
}

std::vector<Token> Tokenizer::tokenize(std::fstream & file) const {
    std::vector<Token> output;
    std::unordered_map<std::string, WordCounterToken> wordCounter;
    std::string line;
    while(std::getline(file, line)) {
        std::stringstream stream(line);
        std::string secStr;
        stream >> secStr;
        secStr.pop_back();
        Section section;
        std::cout << "Parsing section: " << secStr << std::endl;
        if (secStr == "Title") 
          section = Section::Title;
        else if (secStr == "Main")
          section = Section::Main;
        else if (secStr == "Body")
          section = Section::Body;
        else if (secStr == "H1")
          section = Section::H1;
        else if (secStr == "H2")
          section = Section::H2;
        else if (secStr == "H3")
          section = Section::H3;
    }
    //insert into output vector
    for (auto &[w, tok]: wordCounter) {
        output.push_back(Token(w, scoreToken(tok)));
    }
    //sort by how many times occured
    std::sort(output.begin(), output.end(), [](const Token & t1, const Token & t2) {
        return t1.getScore() < t2.getScore();
    });
    return output;
}
