#pragma once
#include <curl/curl.h>
#include <string>

class HTTPClient {
    class Response {
        public:
          int status;
          std::string body;
    };
    public:
      HTTPClient();
      HTTPClient(HTTPClient &) = delete;
      [[nodiscard]] Response Visit(const std::string &) const;
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
