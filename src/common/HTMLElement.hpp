#pragma once

#include <string>
#include <vector>

class Element {
  public:
    struct Attribute {
      public:
        std::string Name;
        std::string Value;
    };
    Element(const std::string&);
    Element(const Element&) = default;
    ~Element() = default;
    [[nodiscard]] const std::string& getText() const;
    void addText(const std::string&);
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] const std::vector<Attribute> & getAttributes() const;
    void addAttribute(Attribute);
    [[nodiscard]] std::string getAttribute(const std::string&) const;
  private:
    void printAttributes();
    std::string m_Name;
    std::string m_Text = "";
    std::vector<Attribute> m_Attributes = {};

  //just for testing
  friend class HTMLElementTest;
  friend class HTMLParserTest;
};
