#include "RobotsParserTest.cpp"
#include "HTMLElementTest.cpp"
#include "HTMLParserTest.cpp"

#include <cassert>
#include <iostream>


int main () {
  std::cout << "Testing Program:" << std::endl;

  RobotsParserTest::Run();
  HTMLElementTest::Run();
  HTMLParserTest::Run();

  return 0;
}
