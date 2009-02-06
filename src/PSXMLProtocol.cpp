#include "PSXMLProtocol.h"

#include <arpa/inet.h>
using namespace std;
using namespace psxml;
using namespace xmlpp;

unsigned int PSXMLProtocol::_process_frame(const char * in,unsigned int size) {
  unsigned int read = 0;
  unsigned int offset = 0;
  // calculate any offset
  while(size - offset > 12 
    && string(in+offset,5)!="psxml") {
    offset++;
  }
  if( string(in+offset,5)=="psxml" && size-offset >= 12) {
    unsigned int payload_size = ntohl(
      *reinterpret_cast<const uint32_t*>(in+offset+8));
    if(size-offset-12 >= payload_size) {
      // do the xml parsing
      _parser.parse_memory_raw(reinterpret_cast<const unsigned char *>(
        in+offset+12),payload_size);
      _decoder_output.push_back(_parser.get_document()->get_root_node());
      // we just processed a whole bunch of bytes
      read = offset+12+payload_size;
    }
  }
  return read;
}

vector<Element*> PSXMLProtocol::decode(const char * in, unsigned int size) {
  // 1) Append start buffer to the end of the decder vector
  // make the vector big enough to handle the data
  unsigned int vector_size = _decoder_residual.size();
  _decoder_residual.resize(vector_size+size);
  //append
  memcpy(&_decoder_residual[0] + vector_size,in,size);
  // 3) clear the _decoder_output
  _decoder_output.clear();
  // 4) call _process_frame
  const char * start = &_decoder_residual[0];
  unsigned int s = _decoder_residual.size();
  unsigned int p=0;
  unsigned int r = 0;
  do {
    r = _process_frame(start,s);
    s-=r;
    start+=r;
    p+=r;
  } while(r!=0);
  // the result is now sitting in _decoder_output

  // 4) reset the residual for future iterations
  memcpy(&_decoder_residual[0], start, _decoder_residual.size()-p);
  _decoder_residual.resize(_decoder_residual.size()-p);
 
  // 5) we're done
  return _decoder_output;
} 
