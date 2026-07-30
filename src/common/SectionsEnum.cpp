#include "SectionsEnum.hpp"

Section getSection(const std::string & sectionString) {
    Section section;
    if (sectionString == "Title") 
      section = Section::Title;
    else if (sectionString == "URL")
      section = Section::URL;
    else if (sectionString == "Lang")
      section = Section::Lang;
    else if (sectionString == "Main")
      section = Section::Main;
    else if (sectionString == "Body")
      section = Section::Body;
    else if (sectionString == "H1")
      section = Section::H1;
    else if (sectionString == "H2")
      section = Section::H2;
    else if (sectionString == "H3")
      section = Section::H3;
    else {
      section = Section::Wrong;
    }
    return section;
}

