#include "crawler/Crawler.hpp"
#include <thread>

int main () {
  using namespace std::chrono_literals;
  Crawler c{};
  c.setUserAgent("HobbyProjct: search engine");
  c.setDownloadDirectory("./dowloaded");
  c.registerWorker("https://www.nytimes.com/");
  c.registerWorker("https://en.wikipedia.org/wiki/United_States");
  //c.registerWorker("https://www.seznamzpravy.cz/");
  for (int i=0; i<100; i++)
      c.registerWorker();
  c.start();
  std::this_thread::sleep_for(10000s);
  c.stop();
  return 0;
}
