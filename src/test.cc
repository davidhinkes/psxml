#include "PSXMLProtocol.h"
#include "PSXMLEngine.h"
#include "PSXMLServer.h"

#include <iostream>
#include <arpa/inet.h>
#include <libxml++/libxml++.h>
#include <cassert>
using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;

struct msg {
  char name[8];
  uint32_t size;
  char payload[1024];
  };

struct n_msg {
    msg a[7];
};

int main() {
  msg m = {"psxml",ntohl(16),"<elem>hi</elem>"};
  n_msg n_m;
  for(unsigned int i = 0; i < 7; i++)
    n_m.a[i] = m;
  PSXMLProtocol p;
  vector<shared_ptr<Document> > es = p.decode((char*)&n_m,sizeof(n_msg));
  for(unsigned int i = 0; i < es.size(); i++) {
    assert(es[i] != NULL);
    p.encode(es[i].get());
    cout<<"encode bytes waiting: "<<p.pull_encoded_size()<<endl;
  }
  cout<<"Size: "<<es.size()<<endl;
  PSXMLServer s(10000);
  s.run();
  return 0;
}
