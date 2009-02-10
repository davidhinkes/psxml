/*
 * PSXMLServer
 * Class which provides PSXML server logic
 */

#ifndef _PSXML_PSXMLSERVER_H_
#define _PSXML_PSXMLSERVER_H_

#include "PSXMLProtocol.h"

#include <sys/select.h>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace psxml {
  class PSXMLServer {
  public:
    /*
     * initialize the server based on a port number
     */
    PSXMLServer(uint16_t port);
    /*
     * start the server, never return
     */
    void run();
  private:
    void _deal_with_sockets();
    void _update_max_fd(int fd);
    void _update_max_fd();
    void _route_xml(std::vector<boost::shared_ptr<xmlpp::Document> > docs);
    fd_set _read,_write,_exception;
    // fs -> protocol
    std::map<int,PSXMLProtocol*> _protocols;
    // fd of main server socket
    int _fd;
    int _max_fd;
  };
}

#endif
