#include "PSXMLProtocol.h"

#include <arpa/inet.h>
#include <cassert>

using namespace std;
using namespace psxml;
using namespace xmlpp;
using namespace boost;

// module local helper structure, PSXML Protocol's header frame
struct __psxml_header_t {
  char id[8];
  uint32_t size;
};

unsigned int PSXMLProtocol::_process_frame(const char * in,unsigned int size) {
  unsigned int read = 0;
  unsigned int offset = 0;
  // calculate any offset
  while(size - offset > 12 
    && string(in+offset,5)!="psxml") {
    offset++;
  }
  if(size-offset >= 12 && string(in+offset,5)=="psxml") {
    unsigned int payload_size = ntohl(
      *reinterpret_cast<const uint32_t*>(in+offset+8));
    if(size-offset-12 >= payload_size) {
      // do the xml parsing
      _parser.parse_memory_raw(reinterpret_cast<const unsigned char *>(
        in+offset+12),payload_size);
      // if the XML was parsed correctly, append it to the vector
      if(_parser) {
        shared_ptr<Document> doc(new Document());
        doc->create_root_node_by_import(
	  _parser.get_document()->get_root_node());
        _decoder_output.push_back(doc);
        // we just processed a whole bunch of bytes
        read = offset+12+payload_size;
      } else {
      }
    }
  }
  return read;
}

vector<shared_ptr<Document> > PSXMLProtocol::decode(const char * in,
  unsigned int size) {
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

unsigned int PSXMLProtocol::pull_encoded_size() const {
  return _encoder_residual.size();
}

const char * PSXMLProtocol::pull_encoded() {
  return &_encoder_residual[0];
}

void PSXMLProtocol::pull_encoded(unsigned int bytes) {
  // if bytes are > residual's size, something is wrong
  unsigned int s = _encoder_residual.size();
  assert(bytes <= s);
  memcpy(&_encoder_residual[0], &_encoder_residual[0]+bytes,s-bytes);
  _encoder_residual.resize(s-bytes);
}

void PSXMLProtocol::encode(Document * in) {
  unsigned int s = _encoder_residual.size();
  Glib::ustring out = in->write_to_string();
  _encoder_residual.resize(s+out.bytes()+sizeof(__psxml_header_t));
  __psxml_header_t h = { "psxml",htonl(out.bytes()) };
  memcpy(&_encoder_residual[0]+s,&h,sizeof(__psxml_header_t));
  memcpy(&_encoder_residual[0]+s+sizeof(__psxml_header_t),
    out.data(),out.bytes());
}
