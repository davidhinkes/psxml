/*
 * Parent class which encodes and decodes the PSXML wire protcol.
 * See doc/protocol.txt for details.
 */

#ifndef _PSXML_PSXMLPROTOCOL_H_
#define _PSXML_PSXMLPROTOCOL_H_

#include <vector>
#include <libxml++/libxml++.h>
#include <boost/shared_ptr.hpp>

namespace psxml {
  class PSXMLProtocol {
  public:
    /*
     * inject bytes off-the-wire into the decoder and return a list
     * of PSXML messages
     */
    std::vector<boost::shared_ptr<xmlpp::Document> > decode(
      const char * in, unsigned int size);
    /*
     * inject PSXML message to be serialized
     */
    void encode(xmlpp::Document * in);
    /*
     * get the number of bytes ready to be outputted from the encoder
     */
    unsigned int pull_encoded_size() const;
    /*
     * get a specified number of bytes from the encoder
     * Note: the char * lives within the PXMLPotocol object, no
     * need to delete after use.
     */
    const char * pull_encoded();

    /*
     * tell the encoder how may bytes were successfully removed
     */
    void pull_encoded(unsigned int bytes);

  private:
    unsigned int _process_frame(const char * in, unsigned int size,
      std::vector<boost::shared_ptr<xmlpp::Document> > & docs);
    std::vector<char> _decoder_residual;
    std::vector<char> _encoder_residual;
    xmlpp::DomParser _parser;
  };
}

#endif
