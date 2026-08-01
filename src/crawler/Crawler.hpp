#pragma once
#include "common/ThreadSaveQueue.hpp"
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <filesystem>
#include <mutex>
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
      void enqueueURL(std::string);
      ThreadSaveQueue<std::string> m_UrlsQ = {};
      std::set<std::string> m_Seen = {};
      std::mutex m_SeenMutex{};
      std::map<std::string, RobotsParser> m_RobotsTxts = {}; //domain to Parser
      std::filesystem::path m_DownloadDirectory = {};
      HTTPClient m_Client{};
      HTMLParser m_HTMLParser{};
};
