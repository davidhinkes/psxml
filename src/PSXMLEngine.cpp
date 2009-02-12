#include "PSXMLEngine.h"

using namespace xmlpp;
using namespace psxml;
using namespace std;
using namespace boost;

PSXMLEngine::PSXMLEngine(): _psxml_ns("http://www.psxml.org/PSXML-0.1") {
}

void PSXMLEngine::publish(NodeSet nodes,
  map<int,PSXMLProtocol* > & clients) {
  for(unsigned int i = 0; i < nodes.size(); i++) {
    for(map<int,list<XPathExpression> >::const_iterator it = 
      _subscriptions.begin(); it != _subscriptions.end(); it++) {
      for(list<XPathExpression>::const_iterator j = it->second.begin();
        j != it->second.end(); j++) {
       
        NodeSet pub_list = nodes[i]->find(j->expression,j->ns);
        _publish(pub_list,clients[it->first]);
      }
    }
  }
}
 
void PSXMLEngine::subscribe(int fd, list<XPathExpression> exps) {
  _subscriptions[fd]=exps;
}

void PSXMLEngine::_publish(const NodeSet & nodes, 
  PSXMLProtocol*  client) {
  for(NodeSet::const_iterator it = nodes.begin(); it!=nodes.end(); it++) {
    shared_ptr<Document> doc(new Document);
    Element * e = NULL;
    e = doc->create_root_node("Data",PSXMLEngine::_psxml_ns,"psxml");
    e->import_node(*it);
    client -> encode ( doc );
  }
}
