#include "crawler/Crawler.hpp"
#include <thread>

int main () {
  using namespace std::chrono_literals;
  Crawler c{};
  c.setUserAgent("MyBot");
  c.setDownloadDirectory("./downloaded/");
  c.registerWorker("https://www.wikipedia.org/");
  c.start();
  std::this_thread::sleep_for(10s);
  c.stop();
  return 0;
}
