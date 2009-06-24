#include <psxml.h>
#include <libxml++/libxml++.h>
#include "PSXMLProtocol.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <sstream>
#include <stdexcept>

#include <sys/select.h>
#include <sys/un.h>
#include <cstdlib>

using namespace psxml;
using namespace xmlpp;
using namespace std;
using namespace boost;
Connection::Connection() {
  _pnm["psxml"]="http://www.psxml.org/PSXML-0.1";
  _fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  string path="/tmp/psxml-"+string(getenv("USER")); 
  sockaddr_un addr = {AF_UNIX,""};
  memcpy(addr.sun_path,path.c_str(),path.size());
  assert(connect(_fd,reinterpret_cast<sockaddr*>(&addr),
    sizeof(sockaddr_un)) == 0);
}
Connection::Connection(sockaddr_in addr) {
  _pnm["psxml"]="http://www.psxml.org/PSXML-0.1";
  _fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  assert(connect(_fd,reinterpret_cast<sockaddr*>(&addr),
    sizeof(sockaddr_in)) == 0);
}
Connection::Connection(const std::string & host,unsigned short port) {
  _pnm["psxml"]="http://www.psxml.org/PSXML-0.1";
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  struct addrinfo * info =  NULL;
  struct addrinfo guess;
  memset(&guess,0,sizeof(struct addrinfo));
  guess.ai_family = AF_UNSPEC;
  guess.ai_socktype = SOCK_STREAM;
  stringstream ss;
  ss << port;
  getaddrinfo(host.c_str(),ss.str().c_str(),&guess,&info);
  struct addrinfo * it = NULL;
  for(it = info; it != NULL; it = it->ai_next) {
    _fd = socket(it->ai_family,it->ai_socktype,it->ai_protocol);
    if (_fd < 0)
      continue;
    int con = connect(_fd,it->ai_addr,it->ai_addrlen);
    if (con < 0)
      continue;
    // if we get here, we're done!
    break;
  }
  freeaddrinfo(info);
  if (it == NULL) {
    throw runtime_error("could not connect");
  }
}
void Connection::publish(const list<Element*> & elems) {
  _protocol.publish(elems);
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
  _protocol.subscribe(xpaths);
  _send_socket_io();
}
void Connection::unsubscribe() {
  _protocol.unsubscribe();
  _send_socket_io();
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
      if (bytes_read <=0 )
        throw runtime_error("server disconnect");
      // TODO: deal with read=0 case
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

int Connection::fd() const {
  return _fd;
}
