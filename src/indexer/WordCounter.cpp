#include "indexer/WordCounter.hpp"
#include "common/SectionsEnum.hpp"

void WordCounterToken::incSection(Section section) {
  (*this).m_OccuredInSections[static_cast<size_t>(section)]++;
}

size_t WordCounterToken::getSectionCount(Section s) const {
  return m_OccuredInSections[static_cast<size_t>(s)];
}
