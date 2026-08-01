#pragma once
#include "common/ThreadSafe.hpp"
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <filesystem>
#include <string>
#include <chrono>


//TODO: make the robots.txt work async
//TODO: make scheduler

class Crawler {
    using TimePoint = std::chrono::steady_clock::time_point;
    public:
      class Worker {
        public:
          Worker(const std::filesystem::path&,
                 std::shared_ptr<ThreadSafe::Set<std::string>>,
                 std::shared_ptr<ThreadSafe::Queue<std::string>>,
                 std::shared_ptr<ThreadSafe::Map<std::string, TimePoint>>,
                 const std::string&,
                 std::string);
          Worker(const Worker&) = delete;
          ~Worker() = default;
          void crawl();
          void setId(size_t);
          size_t getId() const;
        private:
          void enqueueURL(std::string);
          bool allowedURL(const std::string&);
          void saveToFile(const std::string&, const std::string&) const;
          bool canScrape(const std::string&);
          void markDomain(const std::string&);

          size_t m_Id;
          HTTPClient m_Client{};
          HTMLParser m_HTMLParser{};
          std::filesystem::path m_DownloadDirectory;
          std::shared_ptr<ThreadSafe::Set<std::string>> m_Seen;
          std::shared_ptr<ThreadSafe::Queue<std::string>> m_UrlsQ;
          std::shared_ptr<std::map<std::string, RobotsParser>> m_RobotsTxts = {}; //domain to Parser
          std::shared_ptr<ThreadSafe::Map<std::string, TimePoint>> m_DomainsCooldown = {};
          std::unordered_map<std::string, TimePoint> m_DomainsCache = {};
          std::vector<std::string> m_UrlsBuffer{};
      };
    public:
      Crawler();
      Crawler(const Crawler&) = delete;
      Crawler operator=(const Crawler&) = delete;
      Crawler(Crawler&&) = delete;
      Crawler operator=(Crawler&&) = delete;
      ~Crawler() = default;
      void start();
      void stop();
      void registerWorker(const std::string&);
      void registerWorker();
      void setUserAgent(const std::string&);
      void setDownloadDirectory(const std::filesystem::path&);
    private:
      static std::string getProperURL(const std::string&);
      std::shared_ptr<ThreadSafe::Queue<std::string>> m_UrlsQ;
      std::shared_ptr<ThreadSafe::Set<std::string>> m_Seen;
      std::shared_ptr<ThreadSafe::Map<std::string, RobotsParser>> m_RobotsTxts; //domain to Parser
      std::shared_ptr<ThreadSafe::Map<std::string, TimePoint>> m_DomainsCooldown;

      std::filesystem::path m_DownloadDirectory{};
      std::string m_UserAgent{};
      std::vector<std::unique_ptr<Worker>> m_Workers;
      std::vector<std::thread> m_Threads;
};
