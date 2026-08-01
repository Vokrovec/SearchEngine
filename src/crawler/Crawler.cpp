#include "crawler/Crawler.hpp"
#include "common/HTMLElement.hpp"
#include "crawler/HTTPClient.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <thread>
#include <chrono>
#include <cassert>
#include <utility>
#include <fstream>

Crawler::Crawler()
    : m_UrlsQ(std::make_shared<ThreadSafe::Queue<std::string>>()),
      m_Seen(std::make_shared<ThreadSafe::Set<std::string>>()),
      m_RobotsTxts(std::make_shared<ThreadSafe::Map<std::string, RobotsParser>>()),
      m_DomainsCooldown(std::make_shared<ThreadSafe::Map<std::string, TimePoint>>())
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
                 std::shared_ptr<ThreadSafe::Set<std::string>> seenPtr,
                 std::shared_ptr<ThreadSafe::Queue<std::string>> queuePtr,
                 std::shared_ptr<ThreadSafe::Map<std::string, TimePoint>> cooldownPtr,
                 const std::string& userAgent,
                 std::string seed="")

                : m_UrlsQ(queuePtr),
                  m_Seen(seenPtr),
                  m_DomainsCooldown(cooldownPtr),
                  m_DownloadDirectory(downloadDir)
{
    assert(m_UrlsQ);
    assert(m_Seen);
    assert(m_DomainsCooldown);
    m_Client.SetUserAgent(userAgent);
    if (!seed.empty())
        m_UrlsQ->push(std::move(seed));

    m_HTMLParser.registerCallback("a", [this](const Element& e){
            std::string url = getProperURL(e.getAttribute("href"));
            m_UrlsBuffer.push_back(url);
        });
    m_HTMLParser.registerCallback("link", [this](const Element& e){
            std::string url = getProperURL(e.getAttribute("href"));
            m_UrlsBuffer.push_back(url);
        });
}

bool Crawler::Worker::canScrape(const std::string& url) {
    std::string domain = HTTPClient::getDomain(url);
    auto now = std::chrono::steady_clock::now();
    auto it = m_DomainsCache.find(domain);
    if (it != m_DomainsCache.end())
        return now >= it->second;
        
    auto nextAllowed = m_DomainsCooldown->get(domain);
    now = std::chrono::steady_clock::now();
    if (!nextAllowed)
        return true;
    if (now >= *nextAllowed)
        return true;
    return false;
    /*
    std::string robotstxt = m_Client.Visit("https://" + domain + "/robots.txt");
    auto[it, inserted] = m_RobotsTxts.try_emplace(domain, robotstxt, m_Client.GetUserAgent());
    if (inserted) {
        std::cout << "Created robots parser for " << domain << '\n';
    }
    return it->second.IsPathAllowed(HTTPClient::getPath(url));
    */
}

void Crawler::Worker::saveToFile(const std::string& filename, const std::string& content) const{
    std::filesystem::path workerDir = m_DownloadDirectory / std::filesystem::path("worker"+std::to_string(m_Id));
    std::filesystem::path filepath = workerDir / std::filesystem::path(filename);

    if (!std::filesystem::exists(workerDir)) {
        if (!std::filesystem::create_directories(workerDir)) {
            throw std::runtime_error("Failed to create directory: " + workerDir.string());
        }
    }
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("Failed to open file: " + filepath.string());
    }

    file << content;
    std::cout << "Saved to file: " << filepath << std::endl;
}

void Crawler::Worker::crawl() {
    std::cout << "Worker " << m_Id << " started\n";
    size_t idx = 0;
    while(!m_UrlsQ->isStoped()) {
        idx++;
        std::string url;
        std::vector<std::string> urlsInCooldown{};
        do {
            if (!m_UrlsQ->pop(url))
                continue;
            if (canScrape(url)) 
                break;
            urlsInCooldown.push_back(url);
        } while(true);

        markDomain(HTTPClient::getDomain(url));
        std::cout << "Scraping URL: " << url << std::endl;
        std::string response = m_Client.Visit(url);
        //std::cout << response << std::endl;
        m_HTMLParser.parse(response);

        //push cooldown urls back to queue
        m_UrlsQ->push_vec(urlsInCooldown);
        m_UrlsQ->push_vec(m_UrlsBuffer);
        m_UrlsBuffer = {};
        saveToFile(std::to_string(idx), response);
    }
}

void Crawler::setUserAgent(const std::string& agentName) {
    m_UserAgent = agentName;
}

void Crawler::setDownloadDirectory(const std::filesystem::path& path) {
    m_DownloadDirectory = path;
}

void Crawler::Worker::enqueueURL(std::string url) {
    if (!m_Seen->insert(url))
        return;

    m_UrlsQ->push(std::move(url));
}

void Crawler::start() {
   for (size_t i = 0; i < m_Workers.size(); i++) {
        m_Workers[i]->setId(i);

        m_Threads.emplace_back(
            &Crawler::Worker::crawl,
            m_Workers[i].get()
        );
    }
}

void Crawler::stop() {
    m_UrlsQ->stop();
    for (auto& thread : m_Threads) {
        if (thread.joinable())
            thread.join();
    }
}

void Crawler::registerWorker(const std::string& seed) {
    std::unique_ptr<Worker> worker_ptr = std::make_unique<Worker>(
                                              m_DownloadDirectory, m_Seen, m_UrlsQ, m_DomainsCooldown, m_UserAgent, seed);
    m_Workers.push_back(std::move(worker_ptr));

}
void Crawler::registerWorker() {
    std::unique_ptr<Worker> worker_ptr = std::make_unique<Worker>(
                                              m_DownloadDirectory, m_Seen, m_UrlsQ, m_DomainsCooldown, m_UserAgent);
    m_Workers.push_back(std::move(worker_ptr));

}

void Crawler::Worker::setId(size_t id) {
    m_Id = id;
}
size_t Crawler::Worker::getId() const{
    return m_Id;
}

void Crawler::Worker::markDomain(const std::string& domain) {
    auto cooldown =  std::chrono::steady_clock::now() + std::chrono::seconds(1);
    m_DomainsCache[domain] = cooldown;
    m_DomainsCooldown->insert(domain, cooldown);
}
