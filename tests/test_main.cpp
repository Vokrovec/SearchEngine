#include "RobotsParserTest.cpp"
#include "HTMLElementTest.cpp"

#include <cassert>
#include <iostream>


int main () {
  std::cout << "Testing Program:" << std::endl;

  RobotsParserTest::Run();
  HTMLElementTest::Run();

  return 0;
}
