require 'socket'
require 'ipaddr'

ip = IPAddr.new("225.4.5.6").hton + IPAddr.new("0.0.0.0").hton
sock = UDPSocket.new
sock.setsockopt(Socket::IPPROTO_IP, Socket::IP_ADD_MEMBERSHIP, ip)
sock.bind(Socket::INADDR_ANY, 5000)

loop do
  msg, info = sock.recvfrom(1024)
  puts "MSG: #{msg} from #{info[2]} (#{info[3]})/#{info[1]} len #{msg.size}" 
end

# EOF
