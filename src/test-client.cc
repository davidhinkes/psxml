#include <psxml.h>

#include <iostream>
#include <libxml++/libxml++.h>
#include <cassert>
using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;
int main() {
  Connection con("127.0.0.1",10000);
  list<Element *> l;
  Document d;
  Element * e =d.create_root_node("testdata");
  l.push_back(e);
  con.publish(l);
  return 0;
}
