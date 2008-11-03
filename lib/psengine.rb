#
# PSEngine
#

require 'thread'
require 'xpathengine'

class PSEngine
  def initialize(config = {:Timeout => 10})
    @queues = Hash.new
    @xpath_expressions = Hash.new
    @lock = Mutex.new
    @id = 0
    @last_time = {}
    @config = config
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
      update_last_time(id)
    }
  end
  def publish(xml)
    @lock.synchronize {
      find_expired_ids
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
    @lock.synchronize {
      delete_last_time(id)
    }
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
    @lock.synchronize {
      update_last_time(id)
    }
    return ret
  end
  private
  def update_last_time(id)
    @last_time[id] = Time.now
  end
  def delete_last_time(id)
    @last_time.delete(id)
  end
  def find_expired_ids
    current_time = Time.now
    @last_time.each_key { |id|
      if (current_time - @last_time[id]) > @config[:Timeout]
        puts "Client with id=#{id} has expired"
        prune(id)
      end
    }
  end
  def prune(id)
    @queues.delete id
    # go through all of the xpath expressions
    # and delete this id
    @xpath_expressions.each_key { |key|
      @xpath_expressions[key].delete id
      if @xpath_expressions[key].empty?
        @xpath_expressions.delete key
      end
      @last_time.delete(id)
    }
  end
end
