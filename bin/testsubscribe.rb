#!/usr/bin/ruby
require 'net/http'
require 'uri'
require 'rexml/document'

include REXML

#1: Simple POST
headers = {'Content-Type' => 'text/xml'}

subscribe_payload = '<h:Subscribe xmlns:h="http://asl.boeing.com/XMLPS-0.1"><h:XPath exp="/b:food"><h:Namespace prefix="b">http://test</h:Namespace></h:XPath></h:Subscribe>'


#retreive_payload = '<h:Retrieve id="id1" xmlns:h="http://asl.boeing.com/XMLPS-0.1"/>'


http = Net::HTTP.start('localhost',1080)
http.read_timeout = 0
res = http.post("/",subscribe_payload)
puts res.body
doc = Document.new(res.body)
id = doc.root.text
retreive_payload = '<h:Retrieve id="'+id+'" xmlns:h="http://asl.boeing.com/XMLPS-0.1"/>'
while true
  res = http.post("/",retreive_payload)
  puts res.body
end

