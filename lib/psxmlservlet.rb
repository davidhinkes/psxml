#!/usr/bin/ruby
# David Hinkes
# for
# Acoustics IRAD
#
# PSXML Servlet

require 'webrick'
require 'rexml/document'
require 'rexml/formatters/default'

require 'psengine'
require 'xpathengine'

include REXML
include WEBrick

class PSXMLServlet < HTTPServlet::AbstractServlet
  PSXML_NAMESPACE='http://asl.boeing.com/PSXML-0.1'
  @@formatter = Formatters::Default.new
  @@ps_engine = PSEngine.new
  def initialize(server,dummy)
    super(server)
    @server=server
  end
  def do_POST(req, resp)
    req_elm = Document.new(req.body).root
    if not req_elm.namespace.eql? PSXML_NAMESPACE
      raise HTTPServletError.new
    end
    case req_elm.name
      when "Publish" then 
        do_publish(req_elm)
      when "Subscribe" then 
        @@formatter.write(do_subscribe(req_elm),resp.body)
      when "Retrieve" then 
        @@formatter.write(do_retrieve(req_elm),resp.body)
    end
  end
  def do_retrieve(pub_elm)
    id = pub_elm.attributes['id'].to_i
    doc = Document.new
    root = doc.add_element 'Data'
    root.add_namespace(PSXML_NAMESPACE)
    @@ps_engine.retrieve(id).each { |e|  root << e}
    return root 
  end
  def do_publish(pub_elm)
    pub_elm.elements.each { |e|
      d = Document.new 
      d << e
      @@ps_engine.publish(d)
    }
    return Document.new
  end
  def do_subscribe(pub_elm)
    id = 0
    if pub_elm.attributes.has_key? 'id'
      id = pub_elm.attributes['id'].to_i
    else
      id = @@ps_engine.next_id
    end
    
    exps = []
    XPath.each(pub_elm,"h:XPath",{'h'=>PSXML_NAMESPACE}) { |e|
      nss = {}
      XPath.each(e,"h:Namespace",{'h'=>PSXML_NAMESPACE}) { |ns|
        nss[ns.attributes['prefix']]=ns.text
      }
      exps << XPathEngine.new(e.attributes['exp'],nss) }
    @@ps_engine.subscribe(id,exps)
    doc =  Document.new
    root = doc.add_element 'ID'
    root.add_namespace(PSXML_NAMESPACE)
    root.text = id.to_s
    return doc 
  end

end

