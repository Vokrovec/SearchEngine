#include "common/HTMLParser.hpp"
#include "common/HTMLElement.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <unordered_set>

const std::unordered_set<std::string> HTMLParser::voidTags = {
    "area", "base", "br", "col", "embed",
    "hr", "img", "input", "link", "meta",
    "source", "track", "wbr", "!DOCTYPE", 
    "footer", "svg", "path", "li"
};

bool HTMLParser::parseElement(const std::string& file, std::vector<Element>& elements, size_t & idx, std::string& parrentText) {
    std::string tag = "";
    std::string text = "";
    while (true) {
        if (idx >= file.size())
            return false;
        if (file[idx] == '<')
            break;
        idx++;
    }
    for (; idx<file.size(); idx++) {
        tag += file[idx];
        if (file[idx] == '>')
            break;
    }

    //not a valid tag
    if (Element::getTagName(tag).empty())
        return false;

    Element el(tag);
    if (voidTags.contains(el.getName())) {
        elements.push_back(el);
        idx++;
        return true;
    }
    idx++;
    for (; idx<file.size()-1; idx++) {
        std::string prevText = text;
        size_t prevIndex = idx;

        //is a new tag?
        if (file[idx] == '<') {

            //is a end tag?
            if (file[idx+1] == '/') {
                while(true) {
                    if (idx >= file.size())
                        return false;
                    if (file[idx]=='>') {
                        idx++;
                        parrentText += text + '\n';
                        el.addText(text);
                        elements.push_back(el);
                        return true;
                    }
                    idx++;
                }
            }
            bool isElement = parseElement(file, elements, idx, text);
            if (isElement) continue;
        }

        if (idx >= file.size())
            return false;
        text += file[idx];
    }
    return false;
    //throw std::runtime_error("Not a valid tag: " + tag + ":" + text);
}

void HTMLParser::parse(const std::string& file) const {
    size_t idx = 0;
    std::string parrentText = "";
    std::vector<Element> elements;
    while (idx<file.size()) {
        if (!parseElement(file, elements, idx, parrentText))
            break;
    }

    //call all callbacks
    for (const Element& e: elements) {
        auto it = m_Callbacks.find(e.getName());
        //std::cout << '"' << e.getName() << '"' << std::endl; //DEBUG
        if (it == m_Callbacks.end())
            continue;
        for (const CallbackFunc& cFunc: (*it).second)
        cFunc(e);
    }
}

void HTMLParser::registerCallback(const std::string& elementName, const CallbackFunc& func) {
    m_Callbacks[elementName].push_back(func);
}
