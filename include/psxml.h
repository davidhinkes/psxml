#ifndef __PSXML__H__
#define __PSXML__H__

#include <libxml++/libxml++.h>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

namespace psxml {
  class XPathExpression {
  public:
    Glib::ustring expression;
    xmlpp::Node::PrefixNsMap ns;
    bool operator==(const XPathExpression & rhs) const;
    bool operator<(const XPathExpression & rhs) const;
  private:
    Glib::ustring _normalize() const;
  };

  class Connection {
  public:
    Connection(const std::string & url);
    ~Connection();
    void publish(boost::shared_ptr<xmlpp::Document>  doc);
    void subscribe(const std::list<XPathExpression> & xpath_expressions);
    void unsubscribe();
    std::list<boost::shared_ptr<xmlpp::Document> > run();
  private:
    xmlpp::DomParser _parser;
  };

}

#endif
