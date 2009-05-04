#include "PSXMLProtocol.h"
#include "PSEngine.h"
#include "Server.h"

#include <iostream>
#include <arpa/inet.h>
#include <libxml++/libxml++.h>
#include <cassert>

#include <csignal>
#include <unistd.h>

using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;

//global variable (ooooo, ahhh, scary)

static Server * server = NULL;
void die(int) {
  delete server;
  exit(0);
}
void ok(int) {}

int main() {
  server = new Server(10000);
  // return to the terminal ...
  int id = fork();
  if(id != 0)
    return 0;
 
  signal(SIGINT,die);
  signal(SIGTERM,die);
  signal(SIGQUIT,die);
  // sigpipe is a part of life ...
  // carry on
  signal(SIGPIPE,ok);
  server->run();
  return 0;
}

