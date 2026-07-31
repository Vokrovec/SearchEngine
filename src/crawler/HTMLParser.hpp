#pragma once
#include "common/HTMLElement.hpp"
#include <cstddef>
#include <functional>
#include <map>
#include <unordered_set>
#include <vector>


class HTMLParser {
  using CallbackFunc = std::function<void(const Element&)>;
  public:
    void parse(const std::string&) const;
    void registerCallback(const std::string&, const CallbackFunc&);

  private:
      static bool parseElement(const std::string&, std::vector<Element>&, size_t&, std::string&);
      static const std::unordered_set<std::string> voidTags;
      std::map<std::string, std::vector<CallbackFunc>> m_Callbacks = {};
};

