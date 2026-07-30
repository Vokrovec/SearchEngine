#include "crawler/RobotsParser.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

RobotsParser::Rule::Rule(const std::string & path, bool isAllowed) : m_Path(path), m_IsAllow(isAllowed) {}

bool RobotsParser::Rule::operator<(const RobotsParser::Rule & r) const {
    return m_Path < r.m_Path;
}

const std::string & RobotsParser::Rule::getPath() const {
    return m_Path;
}
bool RobotsParser::Rule::getAllow() const {
    return m_IsAllow;
}

std::string RobotsParser::CleanLine(const std::string& line) {
    std::string cleaned = line;
    size_t commentPos = cleaned.find('#');
    if (commentPos != std::string::npos) {
        cleaned = cleaned.substr(0, commentPos);
    }
    
    // Trim leading and trailing whitespace
    size_t start = cleaned.find_first_not_of(" \t\r\n");
    size_t end = cleaned.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return cleaned.substr(start, end - start + 1);
}

std::string RobotsParser::ToLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    return std::move(str);
}

bool RobotsParser::IsPathAllowed(const std::string& robotsTxtContent, 
                          const std::string& userAgent, 
                          const std::string& path)
{
    std::stringstream ss(robotsTxtContent);
    std::string line;

    std::vector<Rule> specificUserAgentRules;
    std::vector<Rule> wildcardRules;

    bool inSpecificAgentBlock = false;
    bool inWildcardBlock = false;

    std::string targetAgent = ToLower(userAgent);

    while (std::getline(ss, line)) {
        std::string cleaned = CleanLine(line);
        if (cleaned.empty()) continue;

        size_t colonPos = cleaned.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = ToLower(CleanLine(cleaned.substr(0, colonPos)));
        std::string value = CleanLine(cleaned.substr(colonPos + 1));

        if (key == "user-agent") {
            std::string agentValue = ToLower(value);
            
            // Check if this block applies to our specific agent or the wildcard '*'
            inSpecificAgentBlock = (agentValue == targetAgent);
            inWildcardBlock = (agentValue == "*");
        } 
        else if (key == "disallow" || key == "allow") {
            if (value.empty()) continue; // An empty Disallow means "allow everything"

            Rule rule{value, (key == "allow")};

            if (inSpecificAgentBlock) {
                specificUserAgentRules.push_back(rule);
            } else if (inWildcardBlock) {
                wildcardRules.push_back(rule);
            }
        }
    }

    // Specific agent rules override wildcard '*' rules
    const auto& activeRules = !specificUserAgentRules.empty() 
                              ? specificUserAgentRules 
                              : wildcardRules;

    // Evaluate path against active rules (longest matching rule wins)
    bool allowed = true;
    size_t longestMatchLength = 0;

    for (const auto& rule : activeRules) {
        // Check if the requested path starts with the rule's path prefix
        if (path.rfind(rule.getPath(), 0) == 0) { 
            if (rule.getPath().length() >= longestMatchLength) {
                longestMatchLength = rule.getPath().length();
                allowed = rule.getAllow();
            }
        }
    }

    return allowed;
}

