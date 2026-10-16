#include "common/HTMLParser.hpp"
#include "common/HTMLElement.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unordered_set>
#include <iostream>

/* TODO: This file should really be rewriten into a good parser.
 * for purposse of this project, I will let it be, for now, 
 * and maybe rewrite it again later. 
 */

const std::unordered_set<std::string> HTMLParser::voidTags = {
    "area", "base", "br", "col", "embed",
    "hr", "img", "input", "link", "meta",
    "source", "track", "wbr", "!DOCTYPE", 
    "svg", "path"
};

const std::unordered_set<std::string> HTMLParser::HtmlTags = {
    "a", "abbr", "address", "article", "aside", "audio",
    "b", "bdi", "bdo", "blockquote", "body", "button",
    "canvas", "caption", "cite", "code", "colgroup",
    "data", "datalist", "dd", "del", "details", "dfn", "dialog", "div", "dl", "dt",
    "em",
    "fieldset", "figcaption", "figure", "footer", "form",
    "h1", "h2", "h3", "h4", "h5", "h6",
    "head", "header", "hgroup", "html",
    "i", "iframe", "ins", "kbd",
    "label", "legend", "li",
    "main", "map", "mark", "menu", "meter",
    "nav", "noscript",
    "object", "ol", "optgroup", "option", "output",
    "p", "picture", "pre", "progress",
    "q",
    "rp", "rt", "ruby",
    "s", "samp", "script", "search", "section", "select", "slot", "small", "span", "strong", "style", "sub", "summary", "sup",
    "table", "tbody", "td", "template", "textarea", "tfoot", "th", "thead", "time", "title", "tr",
    "u", "ul",
    "var", "video"
};


std::string HTMLParser::getTagName(std::string& tag) {
    std::string name = "";
    bool isEndTag = false;
    for (char c: tag) {
        if (std::isspace(c))
            break;
        if (c == '/') {
            isEndTag = true;
            continue;
        }
        name += c;
    }
    if (!voidTags.contains(name) && !HtmlTags.contains(name)) {
        //std::cout << "Unknown tag: " << name << std::endl;
        return "";
    }
    if (isEndTag)
        name = '/' + name;

    tag = tag.substr(name.size(), tag.size()-name.size());
    return name;
}

void HTMLParser::getAttributes(const std::string& tag, Element& el) {
    std::stringstream  in(tag);
    while (!in.eof()) {
        in >> std::ws;

        if (in.peek() == '>' || in.peek() == '/')
            break;

        std::string name;
        while (std::isalnum(in.peek()) || in.peek() == '-' || in.peek() == ':') {
            name += in.get();
        }

        in >> std::ws;

        if (in.peek() == '=') {
            in.get();              // consume '='
            in >> std::ws;

            char quote = in.peek(); // " or '
            std::string value;
            if (quote != '"' && quote != '\'') {
                while (!in.eof() || !std::isspace(in.peek())) {
                    value += in.get();
                }
            } else {
                in.get();
                while (in.peek() != quote) {
                    value += in.get();
                }
                in.get(); // consume closing quote
            }

            el.addAttribute(Element::Attribute{
                  .Name=name, 
                  .Value=value
            });
        } else {
            if (name == "")
                continue;
            // Boolean attribute (e.g. disabled, checked)
            el.addAttribute(Element::Attribute{
                .Name=name, 
                .Value=""
                });
        }
    }
}

void HTMLParser::callCallbacks(const Element& el, const auto c_it) const {
    for(CallbackFunc func: c_it->second) {
        func(el);
    }
}

bool HTMLParser::parseElement(std::ifstream& file, Element& parrentElement, bool& isClosing) const {
    std::string tag;
    std::getline(file, tag, '>');
    if (tag.empty()) return false;
    if (tag.back() == '/')
        tag = tag.substr(0, tag.size()-1);
    std::string name = getTagName(tag);
    if (name == '/' + parrentElement.getName()) {
        isClosing = true;
    }
    const auto c_it = m_Callbacks.find(name);
    if (c_it == m_Callbacks.end())
        return true;
    Element el(name);
    getAttributes(tag, el);

    if (voidTags.contains(name)) {
        callCallbacks(el, c_it);
        return true;
    }
    //get tag text
    //and get tags inside
    std::string text = "";
    while (!file.eof()) {
        std::string buffer = "";
        std::getline(file, buffer, '<');
        text += buffer;
        const char c = file.peek();
        if (std::isspace(c)) {
            text += file.get();
            continue;
        }
        bool isClosing = false;
        parseElement(file, el, isClosing);
        if (isClosing)
            break;
        parrentElement.addText(" " + text);
    }
    el.addText(text);
    callCallbacks(el, c_it);
    return true;
}

void HTMLParser::parse(const std::string& filename) const {
    std::ifstream file(filename, std::ios_base::in);
    if (!file.is_open())
        throw std::runtime_error("Can't open file: " + filename);

    std::string text = "";
    Element e{"root"};
    bool flag = false;
    while (!file.eof()) {
        file.ignore(MAX_SIZE, '<');
        const char c = file.peek();
        if (std::isspace(c))
            continue;
        parseElement(file, e, flag);
    }
}

void HTMLParser::registerCallback(const std::string& elementName, const CallbackFunc& func) {
    m_Callbacks[elementName].push_back(func);
}
