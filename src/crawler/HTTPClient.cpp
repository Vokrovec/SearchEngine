#include "HTTPClient.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdexcept>
#include <iostream>

// Helper callback function for libcurl to write data into std::string
static size_t WriteCallback(char* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    if (userp) {
        userp->append(contents, totalSize);
        return totalSize;
    }
    return 0;
}

HTTPClient::HTTPClient() {
 
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK)
        throw std::runtime_error("Curl error: " + std::to_string((int)result));
    m_Curl = curl_easy_init();
    curl_easy_setopt(
        m_Curl,
        CURLOPT_WRITEFUNCTION,
        [](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            auto* response = static_cast<std::string*>(userdata);

            size_t bytes = size * nmemb;
            response->append(ptr, bytes);

            return bytes;
        }
    );
}

HTTPClient::~HTTPClient() noexcept {
    curl_easy_cleanup(m_Curl);
}

void HTTPClient::SetUserAgent(const std::string & agent) const {
    curl_easy_setopt(m_Curl, CURLOPT_USERAGENT, agent.c_str());
}

void HTTPClient::Visit(const std::string & URL) const {
    if (!m_Curl)
        throw std::logic_error("HTTPClient was not initialized!");
    std::string response;
    curl_easy_setopt(m_Curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &response);
    CURLcode result = curl_easy_perform(m_Curl);


    if (result != CURLE_OK) {
        std::cout << "Can not visit this page: " << URL 
                  << "\n Error: " << curl_easy_strerror(result) << std::endl;
        return;
    }
    std::cout << "Visited Page: " << URL << std::endl;
    //std::cout << response << std::endl;
}
