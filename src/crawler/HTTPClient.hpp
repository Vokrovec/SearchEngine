#pragma once
#include <curl/curl.h>
#include <string>

class HTTPClient {
    public:
      HTTPClient();
      HTTPClient(HTTPClient &) = delete;
      [[nodiscard]] std::string Visit(const std::string &) const;
      void SetUserAgent(const std::string &);
      std::string GetUserAgent() const;
      void HandleRobots() const;
      static std::string getDomain(const std::string&);
      static std::string getPath(const std::string&);
      ~HTTPClient() noexcept;
    private:
        CURL *m_Curl = nullptr;
        std::string m_UserAgent;
};
