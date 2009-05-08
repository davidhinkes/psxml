/*
 * PSEngine
 * Performs Publish and Subscribe Actions
 */

#ifndef _PSXML_PSENGINE_H_
#define _PSXML_PSENGINE_H_
#include <psxml.h>
#include "PSXMLProtocol.h"
#include <map>
#include <list>
#include <libxml++/libxml++.h>
#include <boost/shared_ptr.hpp>
#include <set>

namespace psxml {
  class PSEngine {
  public:
    PSEngine();
    void publish(const xmlpp::Element * elem,
      std::map<int,PSXMLProtocol* > & clients,bool foreign = false);
    void publish(const xmlpp::NodeSet & nodes,
      std::map<int,PSXMLProtocol* > & clients, bool foreign = false);
    void subscribe(int fd, std::list<XPathExpression> exps,
      bool full_copy = false);
    void remove(int fd);
    std::list<XPathExpression> aggregate_subscriptions();
  private:
    std::map<int,std::list<XPathExpression> > _subscriptions;
    void _publish(const std::set<xmlpp::Node*> & nodes,
      PSXMLProtocol* clients);
    const Glib::ustring _psxml_ns;
    std::map<int,bool> _full_copy;
  };
}

#endif
