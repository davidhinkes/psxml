#include "PSXMLProtocol.h"
#include "PSXMLEngine.h"
#include "PSXMLServer.h"

#include <iostream>
#include <arpa/inet.h>
#include <libxml++/libxml++.h>
#include <cassert>

#include <csignal>

using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;

//global variable (ooooo, ahhh, scary)

static PSXMLServer * server = NULL;
void die(int) {
  delete server;
  exit(0);
}
int main() {
  server = new PSXMLServer(10000);
  signal(SIGINT,die);
  signal(SIGTERM,die);
  signal(SIGQUIT,die);
  server->run();
  return 0;
}

