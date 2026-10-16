#include "crawler/RobotsParser.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cctype>
#include <sstream>
#include <string>

RobotsParser::RobotsParser(
    const std::string& robotsTxt,
    const std::string& userAgent)
    : m_UserAgent(ToLower(userAgent))
{
    std::stringstream ss(robotsTxt);
    std::string line;

    bool inSpecificAgentBlock = false;
    bool inWildcardBlock = false;

    std::vector<Rule> specificRules;
    std::vector<Rule> wildcardRules;

    while (std::getline(ss, line)) {
        std::string cleaned = CleanLine(line);
        if (cleaned.empty())
            continue;

        const auto colon = cleaned.find(':');
        if (colon == std::string::npos)
            continue;

        const std::string key =
            ToLower(CleanLine(cleaned.substr(0, colon)));

        const std::string value =
            CleanLine(cleaned.substr(colon + 1));

        if (key == "user-agent") {
            const std::string agent = ToLower(value);

            inSpecificAgentBlock = (agent == m_UserAgent);
            inWildcardBlock = (agent == "*");
        }
        else if (key == "allow" || key == "disallow") {
            if (value.empty())
                continue;

            Rule rule{value, key == "allow"};

            if (inSpecificAgentBlock)
                specificRules.push_back(std::move(rule));
            else if (inWildcardBlock)
                wildcardRules.push_back(std::move(rule));
        }
    }

    m_Rules = !specificRules.empty()
        ? std::move(specificRules)
        : std::move(wildcardRules);
}

bool RobotsParser::IsPathAllowed(const std::string& path) const {
    if (path == "") throw std::runtime_error("No path specified!");
    bool allowed = true;
    std::size_t longestMatch = 0;

    for (const auto& rule : m_Rules) {
        if (path.rfind(rule.path, 0) != 0)
            continue;

        if (rule.path.size() > longestMatch) {
            longestMatch = rule.path.size();
            allowed = rule.isAllow;
        }
        else if (rule.path.size() == longestMatch && rule.isAllow) {
            // Tie-break: Allow wins for equal-length matches.
            allowed = true;
        }
    }

    return allowed;
}

std::string RobotsParser::CleanLine(const std::string& line) {
    std::string cleaned = line;

    const auto comment = cleaned.find('#');
    if (comment != std::string::npos)
        cleaned.erase(comment);

    const auto start = cleaned.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return {};

    const auto end = cleaned.find_last_not_of(" \t\r\n");
    return cleaned.substr(start, end - start + 1);
}

std::string RobotsParser::ToLower(std::string str) {
    std::transform(
        str.begin(), str.end(), str.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

    return str;
}
