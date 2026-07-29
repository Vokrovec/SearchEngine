#pragma once

#include <string>
#include <array>

enum class Section {
  Title,
  URL,
  Lang,
  Main,
  Body,
  H1,
  H2,
  H3,
  Wrong,

  Count // needs to be always last
};

constexpr auto makeSections()
{
    std::array<Section, static_cast<size_t>(Section::Count)> result{};

    for (size_t i = 0; i < result.size(); i++) {
        result[i] = static_cast<Section>(i);
    }

    return result;
}
constexpr std::array Sections = makeSections();

Section getSection(const std::string &);

constexpr double sectionWeight(Section s) {
    switch (s) {
        case Section::Title:
            return 20.0;
        case Section::H1:
            return 10.0;
        case Section::H2:
            return 8.0;
        case Section::H3:
            return 6.0;
        case Section::Main:
            return 3.0;
        case Section::Body:
            return 2.0;
        default:
            return 1.0;
    }
}
