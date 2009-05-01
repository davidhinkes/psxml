#ifndef __PSXML_XPATHEXPRESSION_H__
#define __PSXML_XPATHEXPRESSION_H__

#include <libxml++/libxml++.h>

namespace psxml {
  class XPathExpression {
  public:
    Glib::ustring expression;
    xmlpp::Node::PrefixNsMap ns;
  private:
  };
}
#endif
