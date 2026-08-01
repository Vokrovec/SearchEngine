#pragma once
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <filesystem>
#include <queue>
#include <string>
#include <set>
#include <map>


//TODO: make the crawler async

class Crawler {
    public:
      Crawler(const std::string&);
      void crawl();
      void setUserAgent(const std::string&);
      void setDownloadDirectory(const std::filesystem::path&);
      void saveToFile(const std::string&, const std::string&) const;
    private:
      static std::string getProperURL(const std::string&);
      bool canScrape(const std::string&);
      std::queue<std::string> m_UrlsQ = {};
      std::set<std::string> m_Visited = {};
      std::map<std::string, RobotsParser> m_RobotsTxts = {}; //domain to Parser
      std::filesystem::path m_DownloadDirectory = {};
      HTTPClient m_Client{};
      HTMLParser m_HTMLParser{};
};
