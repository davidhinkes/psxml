#include <psxml.h>

#include <iostream>
#include <libxml++/libxml++.h>
#include <cassert>
using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;
int main() {
  //Connection con("localhost",10000);
  Connection con;
  list<XPathExpression> exes;
  XPathExpression ex;
  ex.expression = "/x:testdata";
  ex.ns["x"]="http://playboy.com";
  exes.push_back(ex);
  con.subscribe(exes);
  con.subscribe(exes);
  list<Element *> l;
  Document d;
  Element * e =d.create_root_node("testdata","http://playboy.com","la");
  unsigned int size = 1024*1024*5;
  char buf[size];
  for(unsigned int i=0;i<size;i++)
    buf[i]=10;
  buf[size-1]=0;
  e->add_child_text(buf);
  l.push_back(e);
  con.publish(l);
  con.publish(l);
  con.publish(l);
  while(true) {
  list<Element *> elems = con.run();
  for(list<Element*>::const_iterator it = elems.begin();
    it != elems.end(); it++) {
    cout<<(*it)->get_name()<<endl;
  }
  }
  return 0;
}
