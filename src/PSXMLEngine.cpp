#include "PSXMLEngine.h"

using namespace xmlpp;
using namespace psxml;
using namespace std;
using namespace boost;

PSXMLEngine::PSXMLEngine(): _psxml_ns("http://www.psxml.org/PSXML-0.1") {
}

void PSXMLEngine::publish(const Element * pub_elem,
  map<int,PSXMLProtocol* > & clients) {
  // create a document with just the pub_elem
  Document doc;
  doc.create_root_node_by_import(pub_elem);
  for(map<int,list<XPathExpression> >::const_iterator it = 
    _subscriptions.begin(); it != _subscriptions.end(); it++) {
    set<Node*> node_set;
    for(list<XPathExpression>::const_iterator j = it->second.begin();
      j != it->second.end(); j++) {
      // apply xpath expression and get list of nodes to publish
      NodeSet pub_list = doc.get_root_node()->find(j->expression,j->ns);
      for(unsigned int i = 0; i < pub_list.size(); i++) {
        // if this client is in full copy mode, send the ENTIRE
	// XML message, not just the results of the XPATH
	// else, send the normal XPATH output (which may be a subset 
	// of the original XML message)
        if(_full_copy[it->first])
	  node_set.insert(doc.get_root_node());
	else
	  node_set.insert(pub_list[i]);
      }
    }
    _publish(node_set,clients[it->first]);
  }
}

void PSXMLEngine::publish(const NodeSet & nodes,
  map<int,PSXMLProtocol* > & clients) {
  // go through the list of elements and publish
  for(unsigned int i = 0; i < nodes.size(); i++) {
    publish(dynamic_cast<Element*>(nodes[i]),clients);
  }
}
 
void PSXMLEngine::subscribe(int fd, list<XPathExpression> exps,
  bool full_copy) {
  _subscriptions[fd]=exps;
  _full_copy[fd] = full_copy;
}

void PSXMLEngine::remove(int fd) {
  _subscriptions.erase(fd);
  _full_copy.erase(fd);
}

void PSXMLEngine::_publish(const set<Node*> & nodes, 
  PSXMLProtocol*  client) {
  for(set<Node*>::const_iterator it = nodes.begin(); it!=nodes.end(); it++) {
    Document doc;
    Element * e = NULL;
    e = doc.create_root_node("Data",PSXMLEngine::_psxml_ns,"psxml");
    e->import_node(*it);
    client -> encode ( &doc );
  }
}

list<XPathExpression> PSXMLEngine::aggregate_subscriptions() {
  list<XPathExpression> exps;
  for(map<int,list<XPathExpression> >::const_iterator it = 
    _subscriptions.begin(); it != _subscriptions.end(); it++) {
    // the subscriptions only count if they are local (!_full_copy)
    bool b = _full_copy[it->first];
    if(!b) {
      for(list<XPathExpression>::const_iterator j = it->second.begin();
        j != it->second.end(); j++) {
	exps.push_back(*j);
      }
    }
  }
  return exps;
}

