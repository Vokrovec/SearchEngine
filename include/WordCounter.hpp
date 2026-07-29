#pragma once
#include "SectionsEnum.hpp"
#include <array>
#include <string>

class WordCounterToken{
   public:
      WordCounterToken() = default;
      void incSection(Section);
   private:
      std::array<size_t, static_cast<size_t>(Section::Count)> m_OccuredInSections;
};
