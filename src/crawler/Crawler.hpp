#pragma once
#include "common/ThreadSafeQueue.hpp"
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <filesystem>
#include <mutex>
#include <string>
#include <set>
#include <map>


//TODO: make the robots.txt work async
//TODO: make pre domain cooldown

class Crawler {
    public:
      class Worker {
        public:
          Worker(const std::filesystem::path&,
                 std::shared_ptr<std::set<std::string>>,
                 std::shared_ptr<std::mutex>,
                 std::shared_ptr<ThreadSafeQueue<std::string>>,
                 const std::string&,
                 std::string);
          Worker(const Worker&) = delete;
          ~Worker() = default;
          void crawl();
        private:
          void enqueueURL(std::string);
          void saveToFile(const std::string&, const std::string&) const;
          bool canScrape(const std::string&);

          HTTPClient m_Client{};
          HTMLParser m_HTMLParser{};
          std::filesystem::path m_DownloadDirectory;
          std::shared_ptr<std::set<std::string>> m_Seen;
          std::shared_ptr<std::mutex> m_SeenMutex;
          std::shared_ptr<ThreadSafeQueue<std::string>> m_UrlsQ;
      };
    public:
      Crawler();
      Crawler(const Crawler&) = delete;
      Crawler operator=(const Crawler&) = delete;
      Crawler operator=(Crawler&&) = delete;
      Crawler(Crawler&&) = delete;
      ~Crawler() = default;
      void start();
      void stop();
      void registerWorker(const std::string&);
      void registerWorker();
      void setUserAgent(const std::string&);
      void setDownloadDirectory(const std::filesystem::path&);
    private:
      static std::string getProperURL(const std::string&);

      std::shared_ptr<ThreadSafeQueue<std::string>> m_UrlsQ = {};
      std::shared_ptr<std::set<std::string>> m_Seen = {};
      std::shared_ptr<std::mutex> m_SeenMutex{};

      std::map<std::string, RobotsParser> m_RobotsTxts = {}; //domain to Parser
      std::filesystem::path m_DownloadDirectory = {};
      std::string m_UserAgent = {};
      std::vector<std::unique_ptr<Worker>> m_Workers;
};
