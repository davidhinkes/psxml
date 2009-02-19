#include <psxml.h>
#include <libxml++/libxml++.h>
#include "PSXMLProtocol.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <cassert>

#include <sys/select.h>

using namespace psxml;
using namespace xmlpp;
using namespace std;
using namespace boost;

Connection::Connection(const std::string & url,unsigned short port) {
  _pnm["psxml"]="http://www.psxml.org/PSXML-0.1";
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in in = {AF_INET,htons(port), {inet_addr(url.c_str())}};
  assert(connect(_fd,reinterpret_cast<struct sockaddr*>(&in),
    sizeof(struct sockaddr_in))==0);
}

void Connection::publish(const list<Element*> & elems) {
  Document doc;
  Element * root = doc.create_root_node("Publish",
    "http://www.psxml.org/PSXML-0.1","psx");
  for(list<Element*>::const_iterator it = elems.begin(); it!=elems.end();
    it++ ) {
    root->import_node(*it);
  }
  _protocol.encode(&doc);
  _send_socket_io();
}
void Connection::_send_socket_io() {
  unsigned int sent = 0;
  while(sent < _protocol.pull_encoded_size() ) {
    sent += send(_fd,_protocol.pull_encoded()+sent,
      _protocol.pull_encoded_size()-sent,0);
  }
  _protocol.pull_encoded(sent);
}
void Connection::subscribe(const list<XPathExpression> & xpaths) {
  Document doc;
  Element * root = doc.create_root_node("Subscribe",
    "http://www.psxml.org/PSXML-0.1","psx");
  for(list<XPathExpression>::const_iterator it = xpaths.begin();
    it != xpaths.end(); it++) {
    Element * xpath_elem = root->add_child("XPath","psx");
    xpath_elem->set_attribute("exp",it->expression);
    for(Node::PrefixNsMap::const_iterator pns = it->ns.begin();
      pns != it->ns.end(); pns++) {
      Element * ns_elem = xpath_elem->add_child("Namespace","psx");
      ns_elem->set_attribute("prefix",pns->first);
      ns_elem->add_child_text(pns->second);
    }
  }
  _protocol.encode(&doc);
  _send_socket_io();
}
void Connection::unsubscribe() {
  list<XPathExpression> blank;
  subscribe(blank);
} 
Connection::~Connection() {
  // unsubscribe
  unsubscribe();
  close(_fd);
}

list<Element*> Connection::run(unsigned int usec) {
  return _run(usec,true); 
}
list<Element*> Connection::run() {
  return _run(0,false); 
}
list<Element*> Connection::_run(unsigned int usec, bool use_timer) {
  // clear the old docs
  _docs.clear();

  list<Element*> elems;
  struct timeval tv = { 0 , usec };
  while ( tv.tv_usec >= 1000000) {
    tv.tv_sec++;
    tv.tv_usec-=1000000;
  }
  fd_set read, exception;
  bool timeout = false;
  while(elems.empty() && !timeout) {
    FD_ZERO(&read);
    FD_ZERO(&exception);
    FD_SET(_fd,&read);
    FD_SET(_fd,&exception);
    int ret = 0;
    if(use_timer)
      ret = select(_fd+1,&read,NULL,&exception,&tv);
    else
      ret = select(_fd+1,&read,NULL,&exception,NULL);
    assert(ret >=0);
    if (ret == 0)
      timeout=true;
    if(FD_ISSET(_fd,&read)) {
      char buf[1024*64];
      ssize_t bytes_read = recv(_fd,buf,1024*64,0);
      vector<shared_ptr<Document> > ds = _protocol.decode(buf,bytes_read);
      for(unsigned int i = 0; i < ds.size(); i++) {
        _docs.push_back(ds[i]);
        NodeSet s = ds[i]->get_root_node()->find("/psxml:Data/*",_pnm);
        for(NodeSet::const_iterator it = s.begin(); it != s.end(); it++) {
          elems.push_back(dynamic_cast<Element*>(*it));
        }
      }
    }
  }
  return elems;
}
