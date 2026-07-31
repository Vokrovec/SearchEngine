#include "HTTPClient.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdexcept>
#include <iostream>

std::string HTTPClient::getDomain(const std::string& url) {
    CURLU* h = curl_url();
    if (!h)
        throw std::runtime_error("Failed to create CURLU");

    std::string result;

    if (curl_url_set(h, CURLUPART_URL, url.c_str(), 0) == CURLUE_OK) {
        char* host = nullptr;

        if (curl_url_get(h, CURLUPART_HOST, &host, 0) == CURLUE_OK) {
            result = host;
            curl_free(host);
        }
    }

    curl_url_cleanup(h);
    return result;
}

std::string HTTPClient::getPath(const std::string& url) {
    CURLU* urlHandle = curl_url();
    curl_url_set(urlHandle, CURLUPART_URL, url.c_str(), 0);

    char* path = nullptr;
    curl_url_get(urlHandle, CURLUPART_PATH, &path, 0);


    std::string output = path;
    curl_free(path);
    curl_url_cleanup(urlHandle);
    return std::move(output);
}

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
        WriteCallback
    );
    curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_Curl, CURLOPT_MAXREDIRS, 10L);
}

HTTPClient::~HTTPClient() noexcept {
    if (!m_Curl)
        return;
    curl_easy_cleanup(m_Curl);
}

void HTTPClient::SetUserAgent(const std::string & agent) {
    m_UserAgent = agent;
    if (!m_Curl)
        throw std::logic_error("HTTPClient was not initialized!");
    curl_easy_setopt(m_Curl, CURLOPT_USERAGENT, agent.c_str());
}
std::string HTTPClient::GetUserAgent() const {
    return m_UserAgent;
}

std::string HTTPClient::Visit(const std::string & URL) const {
    if (!m_Curl)
        throw std::logic_error("HTTPClient was not initialized!");
    std::string response;
    curl_easy_setopt(m_Curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &response);
    CURLcode result = curl_easy_perform(m_Curl);


    if (result != CURLE_OK) {
        std::cerr << "Can not visit this page: " << URL 
                  << "\n Error: " << curl_easy_strerror(result) << std::endl;
        return "";
    }
    std::cout << "Visited Page: " << URL << std::endl;
    return response;
}
