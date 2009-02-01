#include <psxml.h>

using namespace psxml;
using namespace xmlpp;

Connection::Connection(const std::string & url): _id(-1) {
  _curl = curl_easy_init();
}

Connection::~Connection() {
  // unsubscribe
  unsubscribe();
  curl_easy_cleanup(_curl);
  
}
