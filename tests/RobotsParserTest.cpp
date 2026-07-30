#include "crawler/RobotsParser.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>
//#include <stdexcept>
#include "makros.hpp"
class RobotsParserTest {
  public:
      static void Run() {
          std::cout << "Testing RobotsParser:" << std::endl;
          test_IsPathAllowed();
          test_getFromCache();
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
          //funcionality test
          RobotsParser P(robotsTxt, myBotName);
          assert(P.IsPathAllowed("/blog/posts") == true);
          assert(P.IsPathAllowed("/private/") == true);
          assert(P.IsPathAllowed("/private") == true);
          assert(P.IsPathAllowed("/private/public-file") == true);
          assert(P.IsPathAllowed("/private/idk") == true);
          assert(P.IsPathAllowed("/admin/idk") == false);
          assert(P.IsPathAllowed("/idk") == true);
          assert(P.IsPathAllowed("/admin/idk") == false);
          assert(P.IsPathAllowed("/temp/idk") == false);
          assert(P.IsPathAllowed("/idk/") == true);

          //exception test
          RobotsParser PE("", myBotName);
          TEST_EXCEPTION(PE.IsPathAllowed("/private"), std::runtime_error);

          //cache test
          assert(P.getFromCache("/idk") == std::optional(true));
          assert(P.getFromCache("/idk/") == std::optional(true));
          assert(P.getFromCache("/67/") == std::optional<bool>());
          robotsTxt = R"(
              User-agent: *
              Disallow: /private/
              Allow: /private/public/
          )";

          RobotsParser P2(robotsTxt, myBotName);

          assert(P2.IsPathAllowed("/private/") == false);
          assert(P2.IsPathAllowed("/private/file.txt") == false);

          assert(P2.IsPathAllowed("/private/public/") == true);
          assert(P2.IsPathAllowed("/private/public/file.txt") == true);
          assert(P2.IsPathAllowed("/private/publicity/") == false);

          RobotsParser P3(robotsTxt, myBotName+"idk");

          assert(P3.IsPathAllowed("/private/") == false);
          assert(P3.IsPathAllowed("/private/file.txt") == false);

          assert(P3.IsPathAllowed("/private/public/") == true);
          assert(P3.IsPathAllowed("/private/public/file.txt") == true);
          assert(P3.IsPathAllowed("/private/publicity/") == false);

          robotsTxt = R"(
              User-agent: *
              Disallow: /private/
          )";

          RobotsParser P4(robotsTxt, "*");

          assert(P4.IsPathAllowed("/private/") == false);
          assert(P4.IsPathAllowed("/private/file") == false);
          assert(P4.IsPathAllowed("/private/a/b/c") == false);

          assert(P4.IsPathAllowed("/private2/") == true);
          assert(P4.IsPathAllowed("/private-old/") == true);
          assert(P4.IsPathAllowed("/privates/") == true);

          assert(P4.IsPathAllowed("/private") == true);

          robotsTxt = R"(
              User-agent: *
              Disallow: /everyone/

              User-agent: Googlebot
              Disallow: /google-only/

              User-agent: MyBot
              Disallow: /mybot-only/
          )";

          RobotsParser google(robotsTxt, "Googlebot");
          RobotsParser mybot(robotsTxt, "MyBot");
          RobotsParser other(robotsTxt, "OtherBot");

          assert(google.IsPathAllowed("/google-only/") == false);
          assert(mybot.IsPathAllowed("/mybot-only/") == false);

          assert(other.IsPathAllowed("/everyone/") == false);

          robotsTxt = R"(
              User-agent: *
          )";

          RobotsParser P5(robotsTxt, myBotName);

          assert(P5.IsPathAllowed("/") == true);
          assert(P5.IsPathAllowed("/private/") == true);
          assert(P5.IsPathAllowed("/anything") == true);

          robotsTxt = R"(
              User-agent: *
              Disallow:
          )";

          RobotsParser P6(robotsTxt, "*");

          assert(P6.IsPathAllowed("/") == true);
          assert(P6.IsPathAllowed("/private/") == true);
          assert(P6.IsPathAllowed("/anything") == true);

          robotsTxt = R"(
              User-agent: *
              Disallow: /
          )";

          RobotsParser P7(robotsTxt, "*");

          assert(P7.IsPathAllowed("/") == false);
          assert(P7.IsPathAllowed("/anything") == false);
          assert(P7.IsPathAllowed("/anything/file") == false);

          robotsTxt = R"(
              User-agent: *
              Allow: /
              Disallow: /private/
          )";

          RobotsParser P8(robotsTxt, "*");

          assert(P8.IsPathAllowed("/") == true);
          assert(P8.IsPathAllowed("/public") == true);
          assert(P8.IsPathAllowed("/private/") == false);
          assert(P8.IsPathAllowed("/private/file") == false);

          robotsTxt = R"(
              
                  User-agent:    MyBot
                  Disallow:      /private/
                  Allow:         /private/public/
              
          )";

          RobotsParser P9(robotsTxt, "MyBot");

          assert(P9.IsPathAllowed("/private/") == false);
          assert(P9.IsPathAllowed("/private/public/") == true);

          robotsTxt = R"(
              # This is a comment
              User-agent: *
              Disallow: /private/ # private section
              Allow: /private/public/
          )";

          RobotsParser P10(robotsTxt, "*");

          assert(P10.IsPathAllowed("/private/") == false);
          assert(P10.IsPathAllowed("/private/public/") == true);
      }
      static void test_getFromCache() {
        std::string robotsTxt = R"(
            User-agent: *
            Disallow: /private/
            Disallow: /admin/
            Allow: /public/
        )";

        RobotsParser P(robotsTxt, "*");

        // Initially nothing cached
        assert(P.getFromCache("/private/") == std::optional<bool>());
        assert(P.getFromCache("/public/") == std::optional<bool>());

        // Calling IsPathAllowed populates cache
        assert(P.IsPathAllowed("/private/") == false);
        assert(P.getFromCache("/private/") == std::optional<bool>(false));

        // Allowed path is cached too
        assert(P.getFromCache("/public/") == std::optional<bool>(true));
        assert(P.IsPathAllowed("/public/") == true);

        // Unrelated path gets cached
        assert(P.IsPathAllowed("/hello") == true);
        assert(P.getFromCache("/hello") == std::optional<bool>(true));
      }
};
