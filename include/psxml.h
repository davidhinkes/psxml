#ifndef __PSXML__H__
#define __PSXML__H__

#include <libxml++/libxml++.h>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

namespace psxml {

  class Connection {
  public:
    Connection(const std::string & url);
    ~Connection();
    void publish(xmlpp::Document * doc);
    void subscribe(const std::list<std::string> & xpath_expressions);
    void unsubscribe();
    std::list<boost::shared_ptr<xmlpp::Document> > run();
  private:
    xmlpp::DomParser _parser;
  };

}

#endif
