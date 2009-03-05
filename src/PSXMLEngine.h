/*
 * PSXMLEngine
 * Performs Publish and Subscribe Actions
 */

#ifndef _PSXML_PSXMLEngine_H_
#define _PSXML_PSXMLEngine_H_
#include <psxml.h>
#include "PSXMLProtocol.h"
#include <map>
#include <list>
#include <libxml++/libxml++.h>
#include <boost/shared_ptr.hpp>
#include <set>

namespace psxml {
  class PSXMLEngine {
  public:
    PSXMLEngine();
    void publish(const xmlpp::Element * elem,
      std::map<int,PSXMLProtocol* > & clients);
    void publish(const xmlpp::NodeSet & nodes,
      std::map<int,PSXMLProtocol* > & clients);
    void subscribe(int fd, std::list<XPathExpression> exps);
    void remove(int fd);
  private:
    std::map<int,std::list<XPathExpression> > _subscriptions;
    void _publish(const std::set<xmlpp::Node*> & nodes,
      PSXMLProtocol* clients);
    const Glib::ustring _psxml_ns;
  };
}

#endif
