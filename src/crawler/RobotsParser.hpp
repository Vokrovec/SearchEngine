#pragma once
//#include <iostream>
#include <map>
#include <optional>
#include <string>

class RobotsParser {
  private:
    struct Rule {
        std::string path;
        bool isAllow; // true = Allow, false = Disallow
    };

    // Trim whitespace and remove comments starting with '#'
    [[nodiscard]] static std::string CleanLine(const std::string& line); 

    // Convert string to lowercase for case-insensitive directive matching
    [[nodiscard]] static std::string ToLower(std::string str);

  public:
    RobotsParser(const std::string&, const std::string&);
    RobotsParser(const RobotsParser&)                     = delete;
    RobotsParser operator=(const RobotsParser&)           = delete;
    ~RobotsParser()                                       = default;
    [[nodiscard]] bool IsPathAllowed(const std::string&);

  private:
    [[nodiscard]] std::optional<bool> getFromCache(const std::string&) noexcept;
    std::map<std::string, bool> m_Cache = {};
    std::string m_RobotsTxt;
    std::string m_UserAgent;

    //just for testing
    friend class RobotsParserTest;
};
