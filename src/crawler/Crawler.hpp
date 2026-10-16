#pragma once
#include "common/ThreadSafe.hpp"
#include "crawler/RobotsParser.hpp"
#include "crawler/HTTPClient.hpp"
#include "common/HTMLParser.hpp"
#include <filesystem>
#include <set>
#include <string>
#include <chrono>


//TODO: make the robots.txt work async
//TODO: make scheduler

class Crawler {
  class Worker {

  };
  class Scheduler {

  };
  public:
    Crawler();
    
  private:
    std::set<std::string> m_Visited;
};
