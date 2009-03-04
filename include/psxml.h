#ifndef __PSXML__H__
#define __PSXML__H__

#include <libxml++/libxml++.h>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include <PSXMLProtocol.h>

namespace psxml {
  class XPathExpression {
  public:
    Glib::ustring expression;
    xmlpp::Node::PrefixNsMap ns;
  private:
  };

  class Connection {
  public:
    /*
     * Create a Connection object
     */
    Connection(const std::string & url,unsigned short port);
    /*
     * Create a Connection object via local connection to psxml
     */
    Connection();
    /*
     * Destroy the connection
     */
    ~Connection();
    /*
     * Publish xml data
     */
    void publish(const std::list<xmlpp::Element*> & elems);
    /*
     * Subscribe via xpath
     */
    void subscribe(const std::list<XPathExpression> & xpath_expressions);
    /*
     * relinquish all subscriptions 
     */
    void unsubscribe();
    /*
     * block until data is received or usecs have passed
     */
    std::list<xmlpp::Element*> run(unsigned int usecs);
    /*
     * block until data is received
     */
    std::list<xmlpp::Element*> run();
    /*
     * get the underlying FD
     */
    int fd() const;
  private:
    std::list<xmlpp::Element*> _run(unsigned int usecs, bool use_timer);
    void _send_socket_io();
    xmlpp::DomParser _parser;
    PSXMLProtocol _protocol;
    std::list<boost::shared_ptr<xmlpp::Document> > _docs;
    xmlpp::Node::PrefixNsMap _pnm;
    int _fd;
  };

}

#endif
