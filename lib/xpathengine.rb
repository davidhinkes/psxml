
class XPathEngine
  attr_reader :exp,:namespaces
  def initialize(exp,namespaces)
    @exp = exp
    @namespaces = namespaces
  end
  def each(node)
    XPath.each(node,@exp,@namespaces) { |e| yield e }
  end
  def eql?(rhs)
    return (self == rhs)
  end
  def ==(rhs)
    return (self.class == rhs.class and 
            @exp == rhs.exp and 
            @namespaces == rhs.namespaces)
  end
  def hash
    @exp.hash
  end
end


