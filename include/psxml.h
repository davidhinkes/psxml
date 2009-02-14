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
    Connection(const std::string & url,unsigned short port);
    ~Connection();
    void publish(std::list<xmlpp::Element*>  doc);
    void subscribe(const std::list<XPathExpression> & xpath_expressions);
    void unsubscribe();
    std::list<boost::shared_ptr<xmlpp::Document> > run();
  private:
    void _send_socket_io();
    xmlpp::DomParser _parser;
    PSXMLProtocol _protocol;
    int _fd;
  };

}

#endif
