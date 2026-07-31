#include "crawler/HTMLParser.hpp"
#include "common/HTMLElement.hpp"
#include <string>
#include <vector>
#include <unordered_set>

const std::unordered_set<std::string> HTMLParser::voidTags = {
    "area", "base", "br", "col", "embed",
    "hr", "img", "input", "link", "meta",
    "source", "track", "wbr"
};

void HTMLParser::parse(const std::string& file) const {
    std::vector<Element> elements;
    size_t stackIdx = 0;
    std::string tag = "";
    std::string text = "";
    for (size_t i=0; i<file.size(); i++) {
        if (file[i] != '<') {
            if (stackIdx != 0) 
                text += file[i];
            continue;
        }

        if (stackIdx != 0) elements[stackIdx-1].addText(text);
        while(i<file.size()) {
            tag += file[i];
            if (file[i] == '>') break;
            i++;
        }

        //found end of tag
        if (tag[0] == '<' && tag[1] == '/') {
            stackIdx--;
            continue;
        } 

        //found start of tag
        Element e(tag);
        elements.push_back(e);
        tag = "";

        //it's one line tag
        if (!voidTags.contains(e.getName())) 
            stackIdx++;

    }

    //call all callbacks
    for (const Element& e: elements) {
        auto it = m_Callbacks.find(e.getName());
        if (it == m_Callbacks.end())
            continue;
        for (const CallbackFunc& cFunc: (*it).second)
        cFunc(e);
    }
}

void HTMLParser::registerCallback(const std::string& elementName, const CallbackFunc& func) {
    m_Callbacks[elementName].push_back(func);
}
