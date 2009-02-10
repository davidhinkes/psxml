#include <psxml.h>
#include <libxml++/libxml++.h>

using namespace psxml;
using namespace xmlpp;
using namespace std;


bool XPathExpression::operator==(const XPathExpression & rhs) const {
  return (_normalize() == rhs._normalize());
}
bool XPathExpression::operator<(const XPathExpression & rhs) const {
  return (_normalize() < rhs._normalize());
}

Glib::ustring XPathExpression::_normalize() const {
  Glib::ustring s = expression;
  for(Node::PrefixNsMap::const_iterator it = ns.begin(); it!=ns.end();it++) {
    //TODO: do the replace 
    //s.replace(it->first+string(":"),it->second+string(":"));
  }
  return s;
}
Connection::Connection(const std::string & url) {
}

Connection::~Connection() {
  // unsubscribe
  //unsubscribe();
}
