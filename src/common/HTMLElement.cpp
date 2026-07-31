#include "HTMLElement.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <utility>

std::string Element::getAttribute(const std::string& attr) const {
    for (const auto& a: m_Attributes) {
        if (a.Name == attr)
            return a.Value;
    }
    return "";
}

std::string Element::getTagName(const std::string& tag) {
    if (tag.empty())
        throw std::runtime_error("Invalid (empty) Tag!");
    if (tag[0] != '<')
        throw std::runtime_error("Invalid Tag no < of position 0!");
    std::string name = "";
    size_t idx = 1;
    while (idx < tag.size()) {
        if (!std::isspace(tag[idx]))
            break;
        idx++;
    }
    for (size_t i=idx; i<tag.size(); i++) {
        if (std::isspace(tag[i]) || tag[i] == '>') break;
        name += tag[i];
    }
    if (name == "")
        throw std::runtime_error("Invalid Tag");
    return std::move(name);
}

Element::Element(const std::string& tag) {
    if (tag.empty())
        throw std::runtime_error("Invalid (empty) Tag");
    if (tag[0] != '<')
        throw std::runtime_error("Invalid Tag");
    m_Name = getTagName(tag);
    size_t idx = m_Name.size() + 1;
    char lastChar = '\0';
    std::string attrName = "";
    while (true) {
        if (idx >= tag.size())
            throw std::runtime_error("Invalid Tag: didn't find closing '>': " + tag);
        if (std::isalnum(tag[idx]) || tag[idx] == '-' || tag[idx] == '_') {
            attrName += tag[idx];
            idx++;
            continue;
        }
        if (std::isspace(tag[idx])){
            if (!attrName.empty() && lastChar!='=') {
                addAttribute({.Name = attrName, .Value = ""});
                attrName = "";
            }
            idx++;
            continue;
        }

        if (lastChar == '=') {
            bool isQuoted = false;
            while(true) {
                if (idx >= tag.size())
                    throw std::runtime_error("Invalid Tag: Didn't find opening \" inside of arttibute: " + tag);
                if (tag[idx] == '"') {
                  isQuoted = true;
                  idx++;
                  break;
                }
                if (!std::isspace(tag[idx])) {
                    break;
                }
            }
            lastChar = '\0';
            std::string attrValue = "";
            while(true) {
                if (idx >= tag.size())
                    throw std::runtime_error("Invalid Tag: Didn't find closing \" inside of arttibute: " + tag);
                if (isQuoted && tag[idx] == '"') break;
                if (std::isspace(tag[idx]) || tag[idx] == '>') {
                    break;
                }
                attrValue += tag[idx];
                idx++;
            }

            addAttribute({
                .Name = attrName,
                .Value = attrValue
              });
            attrName = "";
        }


        if (tag[idx] == '>') {
          if (attrName.empty())
              break;
          addAttribute({
              .Name = attrName,
              .Value = ""
              });
          break;
        }
        lastChar = tag[idx];
        idx++;
    }
    if (idx+1 != tag.size())
          throw std::runtime_error("Invalid Tag invalid size!");
}

void Element::printAttributes() {
    for (const Attribute& a : m_Attributes)
        std::cout << "Name: " << a.Name 
                  << " Value: " << a.Value << std::endl;
}

void Element::addText(const std::string& text) {
    m_Text += text;
}

void Element::addAttribute(const Attribute& atribute) {
    m_Attributes.push_back(atribute);
}

const std::string& Element::getText() const {
    return m_Text;
}

const std::string& Element::getName() const {
    return m_Name;
}

std::vector<Element::Attribute> Element::getAttributes() const {
    return m_Attributes;
}
