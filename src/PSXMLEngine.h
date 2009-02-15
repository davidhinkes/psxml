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

namespace psxml {
  class PSXMLEngine {
  public:
    PSXMLEngine();
    void publish(xmlpp::Element * elem,
      std::map<int,PSXMLProtocol* > & clients);
    void publish(xmlpp::NodeSet nodes,
      std::map<int,PSXMLProtocol* > & clients);
    void subscribe(int fd, std::list<XPathExpression> exps);
  private:
    std::map<int,std::list<XPathExpression> > _subscriptions;
    void _publish(const xmlpp::NodeSet & nodes,
      PSXMLProtocol* clients);
    const Glib::ustring _psxml_ns;
  };
}

#endif
