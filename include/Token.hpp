#pragma once
#include <string>

struct Token {
    public:
      Token(const std::string &, size_t);
      size_t getCount() const {
        return m_Count;
      }
    private:
      std::string m_Word;
      size_t m_Count;
      double m_Score;
};
