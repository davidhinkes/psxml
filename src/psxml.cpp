#include <psxml.h>
#include <libxml++/libxml++.h>
#include "PSXMLProtocol.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <cassert>

using namespace psxml;
using namespace xmlpp;
using namespace std;

Connection::Connection(const std::string & url,unsigned short port) {
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in in = {AF_INET,htons(port), {inet_addr(url.c_str())}};
  assert(connect(_fd,reinterpret_cast<struct sockaddr*>(&in),
    sizeof(struct sockaddr_in))==0);
}

void Connection::publish(list<Element*>  elems) {
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
Connection::~Connection() {
  // unsubscribe
  //unsubscribe();
}
