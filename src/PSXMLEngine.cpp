#include "PSXMLEngine.h"

using namespace xmlpp;
using namespace psxml;
using namespace std;
using namespace boost;

void PSXMLEngine::publish(NodeSet nodes,
  map<int,PSXMLProtocol* > & clients) {
  for(unsigned int i = 0; i < nodes.size(); i++) {
    for(map<XPathExpression,list<int> >::const_iterator it = 
      _subscriptions.begin(); it != _subscriptions.end(); it++) {
      NodeSet pub_list = nodes[i]->find(it->first.expression,it->first.ns);
      _publish(pub_list,it->second,clients);
    }
  }
}
 
void PSXMLEngine::subscribe(int fd, list<XPathExpression> exps) {
  for(list<XPathExpression>::const_iterator it = exps.begin();
    it != exps.end(); it++) {

    _subscriptions[*it].push_back(fd);
  }
}

void PSXMLEngine::_publish(const NodeSet & nodes, const list<int> & fds,
  map<int,PSXMLProtocol* > & clients) {
  for(NodeSet::const_iterator it = nodes.begin(); it!=nodes.end(); it++) {
    shared_ptr<Document> doc(new Document);
    Element * e = NULL;
    //e = doc.create_root_node(...);
    e->import_node(*it);
    for(list<int>::const_iterator i=fds.begin(); i!=fds.end(); i++) {
      clients[ *i ] -> encode ( doc );
    }
  }
}
