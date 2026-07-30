#include "crawler/HTTPClient.hpp"

int main () {
  HTTPClient client;
  client.SetUserAgent("Dark Secret Ninja/1.0");
  client.Visit("https://www.wikipedia.org/");
  return 0;
}
