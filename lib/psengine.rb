#
# PSEngine
#

require 'thread'
require 'xpathengine'

class PSEngine
  def initialize
    @queues = Hash.new
    @xpath_expressions = Hash.new
    @lock = Mutex.new
    @id = 0
  end
  def next_id
    ret = 0
    @lock.synchronize {
      ret = @id
      @id = @id+1
    }
    return ret
  end
  def subscribe(id,exps)
    @lock.synchronize {
      prune(id)
      @queues[id]=Queue.new
      exps.each { |exp|
        if not @xpath_expressions.has_key? exp 
          @xpath_expressions[exp] = []
        end
        @xpath_expressions[exp] << id 
      }
    }
  end
  def publish(xml)
    @lock.synchronize {
      @xpath_expressions.each_key{ |exp|
        exp.each(xml) { |r|
          @xpath_expressions[exp].each{ |id|
            @queues[id] << r
          }
        }
      }
    }
  end
  def retrieve(id)
    ret = []
    if not @queues.has_key? id
      @queues[id] = Queue.new
    end
    if not @queues[id].empty?
      while not @queues[id].empty?
        ret << @queues[id].pop 
      end
    else
      ret << @queues[id].pop
    end
    return ret
  end
  private
  def prune(id)
    @queues.delete id
    # go through all of the xpath expressions
    # and delete this id
    @xpath_expressions.each_key { |key|
      @xpath_expressions[key].delete id
      if @xpath_expressions[key].empty?
        @xpath_expressions.delete key
      end
    }
  end
end


