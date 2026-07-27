#include <iostream>
#include <fstream>



int main (int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong number of arguments!\n";
    std::cerr << "Expected name of search file!" << std::endl;
    return 1;
  }
  std::string filename = argv[1];
  std::fstream file;
  file.open(filename);
  if (!file.is_open()) {
    std::cerr << "File cannot be opened!" << std::endl;
    return 1;
  }
  std::string content;
  return 0;
}
