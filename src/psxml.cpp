#include <psxml.h>
#include <libxml++/libxml++.h>

using namespace psxml;
using namespace xmlpp;
using namespace std;

Connection::Connection(const std::string & url) {
}

Connection::~Connection() {
  // unsubscribe
  //unsubscribe();
}
