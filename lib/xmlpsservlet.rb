#!/usr/bin/ruby
# David Hinkes
# for
# Acoustics IRAD
#
# XMLPS Servlet

require 'webrick'
require 'rexml/document'
require 'rexml/formatters/default'

require 'psengine'
require 'xpathengine'

include REXML
include WEBrick

class XMLPSServlet < HTTPServlet::AbstractServlet
  XMLPS_NAMESPACE='http://asl.boeing.com/XMLPS-0.1'
  @@formatter = Formatters::Default.new
  @@ps_engine = PSEngine.new
  def initialize(server,dummy)
    super(server)
  end
  def do_POST(req, resp)
    req_elm = Document.new(req.body).root
    if not req_elm.namespace.eql? XMLPS_NAMESPACE
      raise HTTPServletError.new
    end
    id = req.peeraddr[3]+':'+req.peeraddr[1].to_s 
    case req_elm.name
      when "Publish" then 
        do_publish(req_elm)
      when "Subscribe" then 
        @@formatter.write(do_subscribe(req_elm,id),resp.body)
      when "Retrieve" then 
        @@formatter.write(do_retrieve(id),resp.body)
    end
  end
  def do_retrieve(id)
    doc = Document.new
    root = doc.add_element 'Data'
    root.add_namespace(XMLPS_NAMESPACE)
    @@ps_engine.retrieve(id).each { |e|  root << e}
    return root 
  end
  def do_publish(pub_elm)
    pub_elm.elements.each { |e|
      d = Document.new 
      d << e
      @@ps_engine.publish(d) }
    return Document.new
  end
  def do_subscribe(pub_elm,id) 
    exps = []
    XPath.each(pub_elm,"h:XPath",{'h'=>XMLPS_NAMESPACE}) { |e|
      nss = {}
      XPath.each(e,"h:Namespace",{'h'=>XMLPS_NAMESPACE}) { |ns|
        nss[ns.attributes['prefix']]=ns.text
      }
      exps << XPathEngine.new(e.attributes['exp'],nss) }
    @@ps_engine.subscribe(id,exps)
    doc =  Document.new
    root = doc.add_element 'ID'
    root.add_namespace(XMLPS_NAMESPACE)
    root.text = id.to_s
    return doc 
  end

end

