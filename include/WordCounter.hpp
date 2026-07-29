#pragma once
#include "SectionsEnum.hpp"
#include <array>

class WordCounterToken{
   public:
      WordCounterToken() = default;
      WordCounterToken(WordCounterToken&) = default;
      ~WordCounterToken() = default;
      void incSection(Section);
      size_t getSectionCount(Section) const;
   private:
      std::array<size_t, static_cast<size_t>(Section::Count)> m_OccuredInSections = {};
};
