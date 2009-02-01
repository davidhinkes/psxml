#ifndef __PSXML__H__
#define __PSXML__H__

#include <libxml++/libxml++.h>
#include <curl/curl.h>
#include <list>
#include <string>
namespace psxml {

  class Connection {
  public:
    Connection(const std::string & url);
    ~Connection();
    void publish(xmlpp::Document * doc);
    void subscribe(const std::list<std::string> & xpath_expressions);
    void unsubscribe();
    std::list<xmlpp::Element*> run();
  private:
    size_t _curl_callback(void * ptr, size_t size, size_t nmemb, void * stream);
    xmlpp::DomParser _parser;
    int _id;
    CURL * _curl;
  };


}

#endif
