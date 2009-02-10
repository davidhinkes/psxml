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
    void publish(xmlpp::NodeSet nodes,
      std::map<int,PSXMLProtocol* > & clients);
    void subscribe(int fd, std::list<XPathExpression> exps);
  private:
    std::map<XPathExpression,std::list<int> > _subscriptions;
    void _publish(const xmlpp::NodeSet & nodes,const std::list<int> & fds,
      std::map<int,PSXMLProtocol* > & clients);
  };
}

#endif
