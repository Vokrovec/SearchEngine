#pragma once
#include <string>

struct Token {
    public:
      Token(const std::string &, double);
      [[nodiscard]] double getScore() const;
      [[nodiscard]] const std::string & getWord() const;
    private:
      std::string m_Word;
      double m_Score;
};
