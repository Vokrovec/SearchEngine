#pragma once
#include "common/HTMLElement.hpp"
#include <functional>
#include <map>
#include <unordered_set>


class HTMLParser {
  using CallbackFunc = std::function<void(const Element&)>;
  public:
    void parse(const std::string&) const;
    void registerCallback(const std::string&, const CallbackFunc&);

  private:
      static const std::unordered_set<std::string> voidTags;
      std::map<std::string, std::vector<CallbackFunc>> m_Callbacks;
};

