#pragma once
#include "common/HTMLElement.hpp"
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <queue>
#include <string>
#include <set>
#include <map>


class Crawler {
    public:
      Crawler(const std::string&);
      void crawl();
      void setUserAgent(const std::string&);
    private:
      std::queue<std::string> m_UrlsQ = {};
      std::set<std::string> m_Visited = {};
      std::map<std::string, RobotsParser> m_RobotsTxts = {}; //domain to Parser
      std::vector<Element> m_Links = {};
      HTTPClient m_Client{};
      HTMLParser m_HTMLParser{};
};
