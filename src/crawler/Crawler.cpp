#include "crawler/Crawler.hpp"
#include "common/HTMLElement.hpp"
#include "crawler/HTTPClient.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Crawler::Crawler(const std::string& seed) {
    m_UrlsQ.push(seed);
    m_HTMLParser.registerCallback("a", [this](const Element& e){
            std::string link = e.getAttribute("href");
            std::cout << "Found a: " << link << std::endl;
            m_UrlsQ.push(link);
        });
    m_HTMLParser.registerCallback("link", [this](const Element& e){
            std::string link = e.getAttribute("href");
            std::cout << "Found link: " << link << std::endl;
            m_UrlsQ.push(link);
        });
}

void Crawler::crawl() {
    while(!m_UrlsQ.empty()) {
        std::string url = m_UrlsQ.back();
        std::cout << "Scraping URL: " << url << std::endl;
        m_UrlsQ.pop();
        std::string response = m_Client.Visit(url);
        std::cout << response << std::endl;
        m_HTMLParser.parse(response);
        //TODO: write response to the file
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Crawler::setUserAgent(const std::string& agentName) {
    m_Client.SetUserAgent(agentName);
}


