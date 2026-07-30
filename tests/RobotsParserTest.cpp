#include "crawler/RobotsParser.hpp"
#include <iostream>
#include <cassert>
class RobotsParserTest {
  public:
      static void Run() {
          std::cout << "Testing RobotsParser:" << std::endl;
          test_IsPathAllowed();
          std::cout << "All Tests Passed RobotsParser!" << std::endl;
      }
  private:
      static void test_IsPathAllowed() {
          std::string robotsTxt = R"(
              User-agent: *
              Disallow: /private/
              Allow: /private/public-file.html

              User-agent: MyBot
              Disallow: /admin/
              Disallow: /temp/
          )";

          std::string myBotName = "MyBot";

          assert(RobotsParser::IsPathAllowed(robotsTxt, myBotName, "/admin/login") == false);
          assert(RobotsParser::IsPathAllowed(robotsTxt, myBotName, "/blog/posts") == true);
      }
};
