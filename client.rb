require 'socket'
starttime = Process.clock_gettime(Process::CLOCK_MONOTONIC)

s = TCPSocket.new 'localhost', 8989

s.write("/home/pikku/mock/#{ARGV[0]}.jpg\n")

s.each_line do |line|
    #puts line
end

s.close
endtime = Process.clock_gettime(Process::CLOCK_MONOTONIC)
elapsed = endtime - starttime
puts "Elapsed: #{elapsed} (#{ARGV[0]})"
