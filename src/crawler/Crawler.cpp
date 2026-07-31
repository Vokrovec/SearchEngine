#include "crawler/Crawler.hpp"
#include "common/HTMLElement.hpp"
#include "crawler/HTTPClient.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cassert>
#include <utility>
#include <fstream>

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
    std::cerr << "Unknown format of URL: " << inputURL << std::endl;
    return "";
}

Crawler::Crawler(const std::string& seed) {
    m_UrlsQ.push(seed);
    m_HTMLParser.registerCallback("a", [this](const Element& e){
            std::string link = e.getAttribute("href");
            std::string url = getProperURL(link);
            if (url.empty()) return;
            if (m_Visited.contains(url)) return;
            //std::cout << "Found <a>: " << url << std::endl;
            m_UrlsQ.push(url);
        });
    m_HTMLParser.registerCallback("link", [this](const Element& e){
            std::string link = e.getAttribute("href");
            std::string url = getProperURL(link);
            if (url.empty()) return;
            if (m_Visited.contains(url)) return;
            //std::cout << "Found <link>: " << url << std::endl;
            m_UrlsQ.push(url);
        });
}

bool Crawler::canScrape(const std::string& url) {
    std::string domain = HTTPClient::getDomain(url);
    std::string robotstxt = m_Client.Visit("https://" + domain + "/robots.txt");
    auto[it, inserted] = m_RobotsTxts.try_emplace(domain, robotstxt, m_Client.GetUserAgent());
    if (inserted) {
        std::cout << "Created robots parser for " << domain << '\n';
    }
    return it->second.IsPathAllowed(HTTPClient::getPath(url));
}

void Crawler::saveToFile(const std::string& filename, const std::string& content) const{
    std::filesystem::path filepath = m_DownloadDirectory / std::filesystem::path(filename);
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    file << content;
    std::cout << "Saved to file: " << filepath << std::endl;
}

void Crawler::crawl() {
    size_t idx = 0;
    while(!m_UrlsQ.empty()) {
        idx++;
        std::string url = m_UrlsQ.front();
        m_UrlsQ.pop();
        if (m_Visited.contains(url))
            continue;
        if (!canScrape(url))
            continue;
        m_Visited.insert(url);
        std::cout << "Scraping URL: " << url << std::endl;
        std::cout << "URLs in queue: " << m_UrlsQ.size() << std::endl;
        std::string response = m_Client.Visit(url);
        saveToFile(std::to_string(idx), response);
        m_HTMLParser.parse(response);
        //TODO: write response to the file
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //std::cout << m_UrlsQ.empty() << std::endl;
    }
}

void Crawler::setUserAgent(const std::string& agentName) {
    m_Client.SetUserAgent(agentName);
}

void Crawler::setDownloadDirectory(const std::filesystem::path& path) {
    m_DownloadDirectory = path;
}
