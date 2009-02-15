#include "PSXMLEngine.h"

using namespace xmlpp;
using namespace psxml;
using namespace std;
using namespace boost;

PSXMLEngine::PSXMLEngine(): _psxml_ns("http://www.psxml.org/PSXML-0.1") {
}

void PSXMLEngine::publish(Element * pub_elem,
  map<int,PSXMLProtocol* > & clients) {
  // create a document with just the pub_elem
  Document doc;
  doc.create_root_node_by_import(pub_elem);
  for(map<int,list<XPathExpression> >::const_iterator it = 
    _subscriptions.begin(); it != _subscriptions.end(); it++) {
    for(list<XPathExpression>::const_iterator j = it->second.begin();
      j != it->second.end(); j++) {
      // apply xpath expression and get list of ndes to publish
      NodeSet pub_list = doc.get_root_node()->find(j->expression,j->ns);
      _publish(pub_list,clients[it->first]);
    }
  }
}

void PSXMLEngine::publish(NodeSet nodes,
  map<int,PSXMLProtocol* > & clients) {
  // go through the list of elements and publish
  for(unsigned int i = 0; i < nodes.size(); i++) {
    publish(dynamic_cast<Element*>(nodes[i]),clients);
  }
}
 
void PSXMLEngine::subscribe(int fd, list<XPathExpression> exps) {
  _subscriptions[fd]=exps;
}

void PSXMLEngine::_publish(const NodeSet & nodes, 
  PSXMLProtocol*  client) {
  for(NodeSet::const_iterator it = nodes.begin(); it!=nodes.end(); it++) {
    Document doc;
    Element * e = NULL;
    e = doc.create_root_node("Data",PSXMLEngine::_psxml_ns,"psxml");
    e->import_node(*it);
    client -> encode ( &doc );
  }
}
