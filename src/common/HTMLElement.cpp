#include "HTMLElement.hpp"
#include <iostream>
#include <string>


Element::Element(const std::string& name): m_Name(name){
}

void Element::printAttributes() {
    for (const Attribute& a : m_Attributes)
        std::cout << "Name: " << a.Name 
                  << " Value: " << a.Value << std::endl;
}

const std::string& Element::getText() const {
    return m_Text;
}
void Element::addText(const std::string& text) {
    m_Text += text;
}

const std::string& Element::getName() const {
    return m_Name;
}

const std::vector<Element::Attribute>& Element::getAttributes() const {
    return m_Attributes;
}
std::string Element::getAttribute(const std::string& name) const {
    for (const auto& a: m_Attributes) {
        if (a.Name == name)
            return a.Value;
    }
    return "";
}

void Element::addAttribute(Attribute a) {
    m_Attributes.emplace_back(std::move(a));
}
