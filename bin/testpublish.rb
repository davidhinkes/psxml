#!/usr/bin/ruby
require 'net/http'
require 'uri'
require 'rexml/document'

include REXML

#1: Simple POST
headers = {'Content-Type' => 'text/xml'}

payload = '<h:Publish xmlns:h="http://asl.boeing.com/XMLPS-0.1"><food xmlns="http://test">'+'p'*8192+'</food></h:Publish>'

elm=Document.new(payload).root


http = Net::HTTP.start('localhost',1080)
res = http.post("/",payload)
puts res.body

