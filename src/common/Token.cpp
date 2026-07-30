#include "Token.hpp"
#include "SectionsEnum.hpp"

Token::Token(const std::string & word, double score) : m_Word(word), m_Score(score) {}

double Token::getScore() const {
    return m_Score;
}
const std::string & Token::getWord() const {
    return m_Word;
}
