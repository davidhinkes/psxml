#include "Server.h"
#include <libxml++/libxml++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <ifaddrs.h>
#include <cerrno>
#include <cassert>
#include <cstdlib>

using namespace std;
using namespace psxml;
using namespace boost;
using namespace xmlpp;
using namespace Glib;

Server::Server(uint16_t port): _port(port) {
  //setup the external socket
  _external_fd = socket(AF_INET, SOCK_STREAM,0);
  assert(_external_fd > 0);
  int one = 1;
  assert(setsockopt(_external_fd,SOL_SOCKET,SO_REUSEADDR,
    &one, sizeof(int)==0));
  _max_fd=_external_fd;
  sockaddr_in addr = { AF_INET, htons(port), {INADDR_ANY} };
  int b = bind(_external_fd, reinterpret_cast<const sockaddr*>(&addr),
    sizeof(sockaddr_in));
  assert(b==0);
  assert(listen(_external_fd,1024)==0);
  
  //setup the unix socket
  _local_fd = socket(AF_LOCAL,SOCK_STREAM,0);
  //assert(setsockopt(_local_fd,SOL_SOCKET,SO_REUSEADDR,
  //  &one, sizeof(int)==0));
  _path="/tmp/psxml-"+string(getenv("USER")); 
  sockaddr_un un = { AF_UNIX, "" };
  memcpy(un.sun_path,_path.c_str(),_path.size());
  assert(bind(_local_fd, reinterpret_cast<const sockaddr*>(&un), 
    sizeof(sockaddr_un)) == 0);
  assert(listen(_local_fd,1024)==0);

  //setup the discovery socket
  _discovery_fd = socket(AF_INET,SOCK_DGRAM,0);
  assert(setsockopt(_discovery_fd,SOL_SOCKET,SO_REUSEADDR,
    &one, sizeof(int)==0));
 

  // reuse addr
  b = bind(_discovery_fd, reinterpret_cast<const sockaddr*>(&addr),
    sizeof(sockaddr_in));
  assert(b==0);
  
  _multicast_addr = inet_addr("226.0.0.1"); 
  struct ip_mreq imreq;
  imreq.imr_multiaddr.s_addr = _multicast_addr;
  imreq.imr_interface.s_addr = htonl(INADDR_ANY);
  unsigned char zero = 0;
  setsockopt(_discovery_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,
    &imreq,sizeof(imreq));
  setsockopt(_discovery_fd,IPPROTO_IP,IP_MULTICAST_LOOP,
    &zero,sizeof(zero));


  // clear the FD lists 
  FD_ZERO(&_read);
  FD_ZERO(&_write);
  FD_ZERO(&_exception);

  // tell the rest of the world that we're alive
  _ping();
}
void Server::run() {
  int ret=0;
  while(true) {
    _deal_with_sockets();
    // wait until something changes
    _update_max_fd();
    ret = select(_max_fd+1,&_read,&_write,&_exception,NULL);
    assert(ret >=0);
  }
}
void Server::_ping() {
  sockaddr_in addr = { AF_INET, htons(_port), {_multicast_addr} };
  ssize_t sent = sendto(_discovery_fd,"psxml",5,0,
    reinterpret_cast<const sockaddr*>(&addr),sizeof(addr));
  assert(sent==5);
}

void Server::_deal_with_sockets() {
  set<int> delete_list;
  // check for errors
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    if(FD_ISSET(it->first,&_exception) != 0) {
      // something bad happended!
      // TODO: log
      delete_list.insert(it->first);
    }
  }
  // see if there are any awaiting new sockets on the external 
  // sever socket
  if(FD_ISSET(_external_fd,&_read)!=0) {
    int new_fd = accept(_external_fd,NULL,NULL);
    assert(new_fd > 0);
    _protocols[new_fd] = new PSXMLProtocol;
  }
  // see if there are any awaiting new sockets on the local 
  // sever socket
  if(FD_ISSET(_local_fd,&_read)!=0) {
    int new_fd = accept(_local_fd,NULL,NULL);
    assert(new_fd > 0);
    _protocols[new_fd] = new PSXMLProtocol;
  }
  // see if there are any awating new messages on the discovery
  // UDP socket
  if(FD_ISSET(_discovery_fd,&_read)!=0) {
    sockaddr_in addr;
    socklen_t addr_size;
    char packet[6];
    packet[5] = 0;
    recvfrom(_discovery_fd,packet,5,0,reinterpret_cast<sockaddr*>(&addr),
      &addr_size);
    if(string(packet) =="psxml") {
      // got a discovery packet, see if we already know the existance
      // of the psxmld server by looking at our foreign connections
      bool new_foreign = true;
      for(map<int,uint32_t>::const_iterator it =
        _foreign_connections.begin(); it != _foreign_connections.end();
	it++) {
        if (it->second == addr.sin_addr.s_addr)
	  new_foreign = false;
      }
      if(new_foreign) {
        // new foreign connection ... this is going to be fun!
	int fd = socket(AF_INET,SOCK_STREAM,0);
	// we reuse addr because it's the same for both
	// UDP and TCP
	assert(connect(fd, reinterpret_cast<sockaddr*>(&addr),sizeof(addr))
	  ==0);
        // make a new protocol
	_protocols[fd] = new PSXMLProtocol;
	// keep tabs on this forign connection
        _foreign_connections[fd] = addr.sin_addr.s_addr;
	
	_update_foreign_subscriptions(fd);
	// we just established a new connection .... ping!
	_ping();
      }
    }
  }
 
  // if there is data to be read, read and process (decode)
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    if(FD_ISSET(it->first,&_read) != 0) {
      // we have data (of some sort)
      // I assume that we'll never get over 64K worth
      // of data in one bunch
      char data[1024*64];
      ssize_t rs = recv(it->first,data,1024*64,MSG_DONTWAIT);
      if(rs <= 0) {
        delete_list.insert(it->first);
      } else {
        _route_xml(it->first,it->second->decode(data,rs));
      }
    }
  }
  // if there is data to be written to, do so (encode)
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    // if there is work to be done
    if(FD_ISSET(it->first,&_write) != 0 
      && it->second->pull_encoded_size() > 0 &&
      delete_list.count(it->first)==0) {
      ssize_t ss_ret = -1;
      ssize_t ss = it->second->pull_encoded_size();
      // if there is too much data, throttle back
      bool too_much_data = false;
      
      do {
        ss_ret = send(it->first,it->second->pull_encoded(),
	  ss, MSG_DONTWAIT);
        // reduce the amount we attempt to send by half 
        too_much_data = (errno == EAGAIN || errno == EWOULDBLOCK);
	ss /= 2;
	assert (ss != 0);
      } while (ss_ret <0 && too_much_data);
      if(ss_ret >= 0) {
        // tell the PSXML protocol manager that we did send
        // ss_ret bytes out
        it->second->pull_encoded(ss_ret);
      } else {
        // something bad happend!
        delete_list.insert(it->first);
      }
    } // end "if we have data"
  } // end loop
  // clear the delete list
  for(set<int>::const_iterator it = delete_list.begin();
    it != delete_list.end(); it++) {
    _remove_fd(*it);
  }
  // reset the read and write fds
  FD_ZERO(&_read); FD_ZERO(&_write); FD_ZERO(&_exception);
  FD_SET(_external_fd,&_read);
  FD_SET(_discovery_fd,&_read);
  FD_SET(_local_fd,&_read);
  // set op the FD SET list for another round of processing
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    // want to know if there are reads or exceptions
    FD_SET(it->first,&_read);
    FD_SET(it->first,&_exception);
    // want to know if data can be written only if
    // there is data to write
    if(it->second->pull_encoded_size() > 0)
      FD_SET(it->first,&_write);
  }

} /* end function */

void Server::_update_max_fd(int fd) {
  if (fd > _max_fd)
    _max_fd = fd;
}

void Server::_update_max_fd() {
  _max_fd = max(_local_fd,_external_fd);
  _max_fd = max(_max_fd,_discovery_fd);
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    _update_max_fd(it->first); 
  }
}
void Server::_route_xml(int fd,vector<shared_ptr<Document> > docs) {
  Node::PrefixNsMap pnm;
  pnm["psxml"]="http://www.psxml.org/PSXML-0.1";
  for(unsigned int i = 0; i < docs.size(); i++) {
    shared_ptr<Document> doc = docs[i];
    Element * root = doc->get_root_node();
    // deal with subscribes
    if(root->get_name() == "Subscribe") {
      NodeSet subs = doc->get_root_node()->find(
        "/psxml:Subscribe/psxml:XPath",pnm);
      list<XPathExpression> exps;
      for(unsigned int i =0; i < subs.size(); i++) {
        Element * sub = dynamic_cast<Element*>(subs[i]);
        assert(sub != NULL);
        ustring exp(sub->get_attribute("exp")->get_value());
        NodeSet nss = sub->find("./psxml:Namespace",pnm);
        Node::PrefixNsMap prefix_map;
        XPathExpression xpath;
        xpath.expression = exp;
        for(unsigned int j = 0; j < nss.size(); j++) {
          Element * ns = dynamic_cast<Element*>(nss[j]);
          assert(ns != NULL);
          ustring pf(ns->get_attribute("prefix")->get_value());
          prefix_map[pf] = ns->get_child_text()->get_content();
        }
        xpath.ns = prefix_map;
	exps.push_back(xpath);
      }
      bool foreign = (_foreign_connections.count(fd) > 0); 
      _engine.subscribe(fd,exps,foreign);
      // if our aggregate subscriptions have changed (non-foreign)
      // we tell the other psxmld servers!
      if(!foreign)
        _update_foreign_subscriptions();
    }
    // find any data publishes
    _engine.publish( root->find("/psxml:Publish/*",pnm), _protocols);
  }
}

void Server::_remove_fd(int fd) {
  delete _protocols[fd];
  _protocols.erase(fd);
  _engine.remove(fd);
  _foreign_connections.erase(fd);
  shutdown(fd,SHUT_RDWR);
  close(fd);
}

void Server::_update_foreign_subscriptions(int fd) {
  _protocols[fd]->subscribe(_engine.aggregate_subscriptions());
}

void Server::_update_foreign_subscriptions() {
  for(map<int,uint32_t>::const_iterator it = _foreign_connections.begin();
    it != _foreign_connections.end(); it++) {
    _update_foreign_subscriptions(it->first); 
  }
}

Server::~Server() {
  for(map<int,PSXMLProtocol*>::iterator it = _protocols.begin();
    it != _protocols.end(); it++) {
    close(it->first);
  }
  close(_external_fd);
  close(_local_fd);
  close(_discovery_fd);

  remove(_path.c_str());
}
