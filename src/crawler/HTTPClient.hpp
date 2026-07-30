#pragma once
#include <curl/curl.h>
#include <string>

class HTTPClient {
    public:
      HTTPClient();
      HTTPClient(HTTPClient &) = delete;
      void Visit(const std::string &) const;
      void SetUserAgent(const std::string &) const;
      void HandleRobots() const;
      ~HTTPClient() noexcept;
    private:
        CURL *m_Curl = nullptr;
};
