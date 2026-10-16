#pragma once
#include "common/HTMLElement.hpp"
#include <fstream>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class HTMLParser {
  using CallbackFunc = std::function<void(const Element&)>;
  public:
    void parse(const std::string&) const;
    void registerCallback(const std::string&, const CallbackFunc&);

  private:
      void callCallbacks(const Element& el, const auto c_it) const;
      bool parseElement(std::ifstream&, Element&, bool&) const;
      static std::string getTagName(std::string &);
      std::unordered_map<std::string, std::vector<CallbackFunc>> m_Callbacks = {};

      static constexpr auto MAX_SIZE = std::numeric_limits<std::streamsize>::max();
      static void getAttributes(const std::string&, Element&);
      static const std::unordered_set<std::string> voidTags;
      static const std::unordered_set<std::string> HtmlTags;
};

