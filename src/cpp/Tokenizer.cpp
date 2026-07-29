#include "Tokenizer.hpp"
#include "Token.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <map>


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


bool Tokenizer::isStopWord(const std::string & str) {
  return STOP_WORDS.contains(str);
}

std::vector<Token> Tokenizer::tokenize(std::fstream & file) const {
    std::vector<Token> output;
    std::map<std::string, size_t> wordCounter;
    while(!file.eof()) {
        std::string word;
        file >> word;

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
            wordCounter[word]++;
    }
    //insert into output vector
    for (auto & w: wordCounter) {
        std::string word = w.first;
        size_t cnt = w.second;
        output.push_back(Token(word, cnt));
    }
    //sort by how many times occured
    std::sort(output.begin(), output.end(), [](const Token & t1, const Token & t2) {
        return t1.getCount() < t2.getCount();
    });
    return output;
}
