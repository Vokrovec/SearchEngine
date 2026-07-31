#include "crawler/Crawler.hpp"

int main () {
  Crawler c("https://www.wikipedia.org/");
  c.setUserAgent("MyBot");
  c.crawl();
  return 0;
}
