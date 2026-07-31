#pragma once

#include <string>
#include <vector>

class Element {
  public:
    struct Attribute {
        std::string Name;
        std::string Value;
    };
    Element(const std::string&);
    Element(const Element&) = default;
    ~Element() = default;
    void addText(const std::string&);
    void addAttribute(const Attribute&);
  private:
    static std::string getTagName(const std::string&);
    void printAttributes();
    std::string m_Name;
    std::string m_Text = "";
    std::vector<Attribute> m_Attributes = {};

  //just for testing
  friend class HTMLElementTest;
};
