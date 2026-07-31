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
    void parse(const std::string&);
    void addText(const std::string&);
    void addAttribute(const Attribute&);
    [[nodiscard]] const std::string& getText() const;
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] std::vector<Attribute> getAttributes() const;

  private:
    [[nodiscard]] static std::string getTagName(const std::string&);
    void printAttributes();
    std::string m_Name;
    std::string m_Text = "";
    std::vector<Attribute> m_Attributes = {};

  //just for testing
  friend class HTMLElementTest;
  friend class HTMLParserTest;
};
