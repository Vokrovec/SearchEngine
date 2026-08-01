#include "crawler/Crawler.hpp"
#include "common/HTMLElement.hpp"
#include "crawler/HTTPClient.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <cassert>
#include <utility>
#include <fstream>
#include <mutex>
Crawler::Crawler()
    : m_UrlsQ(std::make_shared<ThreadSafeQueue<std::string>>()),
      m_Seen(std::make_shared<std::set<std::string>>()),
      m_SeenMutex(std::make_shared<std::mutex>())
{
}

std::string Crawler::getProperURL(const std::string& inputURL) {
    if (inputURL.compare(0, 1, "#") == 0) return "";
    if (inputURL.compare(0, 8, "https://") == 0) {
        return inputURL;
    }
    if (inputURL.compare(0, 7, "http://") == 0) {
        return inputURL;
    }
    if (inputURL.compare(0, 2, "//") == 0) {
        return "https:" + inputURL;
    }
    if (inputURL.compare(0, 1, "/") == 0) {
        //let's not care about relative urls for now!
        return "";
    }
    //std::cerr << "Unknown format of URL: " << inputURL << std::endl;
    return "";
}

Crawler::Worker::Worker(const std::filesystem::path& downloadDir,
                 std::shared_ptr<std::set<std::string>> seenPtr,
                 std::shared_ptr<std::mutex> seenMutexPtr,
                 std::shared_ptr<ThreadSafeQueue<std::string>> queuePtr,
                 const std::string& userAgent,
                 std::string seed="")
: m_UrlsQ(queuePtr), m_Seen(seenPtr), m_SeenMutex(seenMutexPtr), m_DownloadDirectory(downloadDir) {
    m_Client.SetUserAgent(userAgent);
    if (!seed.empty())
        m_UrlsQ->push(std::move(seed));

    m_HTMLParser.registerCallback("a", [this](const Element& e){
        std::string url = getProperURL(e.getAttribute("href"));
        std::cout << "Found a" << std::endl;
            if (!url.empty())
                enqueueURL(std::move(url));
        });
    m_HTMLParser.registerCallback("link", [this](const Element& e){
        std::cout << "Found link" << std::endl;
        std::string url = getProperURL(e.getAttribute("href"));

        if (!url.empty())
            enqueueURL(std::move(url));
        });
}

bool Crawler::Worker::canScrape(const std::string& url) {
    return true;
    /*
    std::string domain = HTTPClient::getDomain(url);
    std::string robotstxt = m_Client.Visit("https://" + domain + "/robots.txt");
    auto[it, inserted] = m_RobotsTxts.try_emplace(domain, robotstxt, m_Client.GetUserAgent());
    if (inserted) {
        std::cout << "Created robots parser for " << domain << '\n';
    }
    return it->second.IsPathAllowed(HTTPClient::getPath(url));
    */
}

void Crawler::Worker::saveToFile(const std::string& filename, const std::string& content) const{
    std::filesystem::path filepath = m_DownloadDirectory / std::filesystem::path(filename);
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("Failed to open file: " + filepath.string());
    }

    file << content;
    std::cout << "Saved to file: " << filepath << std::endl;
}

void Crawler::Worker::crawl() {
    size_t idx = 0;
    while(!m_UrlsQ->isStoped()) {
        idx++;
        std::string url;
        if (!m_UrlsQ->pop(url)) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        if (!canScrape(url)) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Scraping not scrape URL: " << url << std::endl;
            continue;
        }
        std::cout << "Scraping URL: " << url << std::endl;
        std::string response = m_Client.Visit(url);
        std::cout << response << std::endl;
        m_HTMLParser.parse(response);
        saveToFile(std::to_string(idx), response);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Crawler::setUserAgent(const std::string& agentName) {
    m_UserAgent = agentName;
}

void Crawler::setDownloadDirectory(const std::filesystem::path& path) {
    m_DownloadDirectory = path;
}

void Crawler::Worker::enqueueURL(std::string url) {
    {
        std::lock_guard lock(*m_SeenMutex);

        if (!m_Seen->insert(url).second)
            return;
    }

    m_UrlsQ->push(std::move(url));
}

void Crawler::start() {
  for (auto& worker_ptr: m_Workers) {
      worker_ptr->crawl();
  }
}

void Crawler::stop() {
    m_UrlsQ->stop();
}

void Crawler::registerWorker(const std::string& seed) {
    std::unique_ptr<Worker> worker_ptr = std::make_unique<Worker>(
                                              m_DownloadDirectory, m_Seen, m_SeenMutex, m_UrlsQ, m_UserAgent, seed);
    m_Workers.push_back(std::move(worker_ptr));

}
void Crawler::registerWorker() {
    std::unique_ptr<Worker> worker_ptr = std::make_unique<Worker>(
                                              m_DownloadDirectory, m_Seen, m_SeenMutex, m_UrlsQ, m_UserAgent);
    m_Workers.push_back(std::move(worker_ptr));

}
