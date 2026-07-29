#pragma once
#include <string>

struct Token {
    public:
      Token(const std::string &, double);
      [[nodiscard]] size_t getScore() const {
        return m_Score;
      }
    private:
      std::string m_Word;
      double m_Score;
};
