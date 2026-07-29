#include "WordCounter.hpp"
#include "SectionsEnum.hpp"
#include <stdexcept>

void WordCounterToken::incSection(Section section) {
  (*this).m_OccuredInSections[static_cast<size_t>(section)]++;
}
