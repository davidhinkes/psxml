#include "PSXMLProtocol.h"
#include "PSXMLEngine.h"
#include "PSXMLServer.h"

#include <iostream>
#include <arpa/inet.h>
#include <libxml++/libxml++.h>
#include <cassert>

using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;

int main() {
  PSXMLServer s(10000);
  s.run();
  return 0;
}

