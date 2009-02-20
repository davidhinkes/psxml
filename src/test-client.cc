#include <psxml.h>

#include <iostream>
#include <libxml++/libxml++.h>
#include <cassert>
using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;
int main() {
  Connection con("localhost",10000);
  list<XPathExpression> exes;
  XPathExpression ex;
  ex.expression = "/x:testdata";
  ex.ns["x"]="http://playboy.com";
  exes.push_back(ex);
  con.subscribe(exes);
  list<Element *> l;
  Document d;
  Element * e =d.create_root_node("testdata","http://playboy.com","la");
  l.push_back(e);
  con.publish(l);
  con.publish(l);
  con.publish(l);
  list<Element *> elems = con.run();
  for(list<Element*>::const_iterator it = elems.begin();
    it != elems.end(); it++) {
    cout<<(*it)->get_name()<<endl;
  }
  return 0;
}
