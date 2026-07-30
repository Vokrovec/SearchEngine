#pragma once
//#include <iostream>
#include <set>
#include <string>

class RobotsParser {
  private:
    class Rule {
        public:
            Rule(const std::string&, bool);
            Rule& operator=(const Rule &)   = default;
            Rule(const Rule &)              = default;
            ~Rule()                         = default;

            //for the set
            [[nodiscard]] bool operator<(const Rule & r) const;

            [[nodiscard]] const std::string & getPath() const;
            [[nodiscard]] bool getAllow() const;
        private:
            std::string m_Path;
            bool m_IsAllow; // true = Allow, false = Disallow
    };

    // Trim whitespace and remove comments starting with '#'
    [[nodiscard]] static std::string CleanLine(const std::string& line); 

    // Convert string to lowercase for case-insensitive directive matching
    [[nodiscard]] static std::string ToLower(std::string str);

  public:
    [[nodiscard]] static bool IsPathAllowed(const std::string& robotsTxtContent, 
                                            const std::string& userAgent, 
                                            const std::string& path);
  private:
  std::set<Rule> m_Cache;

  #ifdef TEST
    friend class RobotsParserTest;
  #endif //TEST
};
