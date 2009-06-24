/*
 * PSXMLServer
 * Class which provides PSXML server logic
 */

#ifndef _PSXML_SERVER_H_
#define _PSXML_SERVER_H_

#include "PSXMLProtocol.h"
#include "PSEngine.h"

#include <sys/select.h>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <netinet/in.h>
#include <arpa/inet.h>

namespace psxml {
  class Server {
  public:
    /*
     * initialize the server based on a port number
     */
    Server(uint16_t port);
    /*
     * kill the server
     */
    ~Server();
    /*
     * start the server, never return
     */
    void run();
  private:
    void _deal_with_sockets();
    void _update_max_fd(int fd);
    void _update_max_fd();
    void _remove_fd(int fd);
    void _route_xml(int fd,
      std::vector<boost::shared_ptr<xmlpp::Document> > docs);
    void _ping();
    void _update_foreign_subscriptions();
    void _update_foreign_subscriptions(int fd);
    fd_set _read,_write,_exception;
    // fd -> protocol
    std::map<int,PSXMLProtocol*> _protocols;
    // fd -> addr (this only works for ipv4 IP
    // addresses)
    std::map<int,uint32_t> _foreign_connections;
    // fd of main server socket
    int _external_fd;
    int _local_fd;
    int _discovery_fd;
    int _max_fd;
    uint16_t _port;
    std::string _path;
    PSEngine _engine;
    in_addr_t _multicast_addr;
  };
}

#endif
