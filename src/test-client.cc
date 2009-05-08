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
  exes.push_back(ex);

  // subscribe to data
  con.subscribe(exes);
  list<Element *> l;
  Document d;
  Element * e =d.create_root_node("testdata","http://playboy.com","la");
  unsigned int size = 512*1024;
  char buf[size];
  for(unsigned int i=0;i<size;i++)
    buf[i]=10;
  buf[size-1]=0;
  e->add_child_text(buf);
  l.push_back(e);

  
  for(unsigned int s = 0; s < 100; s++) {
    list<Element *> elems = con.run(1*1000000);
    if(elems.size() == 0) {
      // didn't get anything ... bad
      cerr<<"test-client didn't get exptected publish data"<<endl;
      for(unsigned int m = 0; m < 64; m++)
        con.publish(l);

    }
    for(list<Element*>::const_iterator it = elems.begin();
      it != elems.end(); it++) {
      cout<<(*it)->get_name()<<endl;
    }
   }
  return 0;
}
