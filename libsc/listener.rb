require 'socket'


s = TCPServer.new('', 5555)
 
while true do
  client = s.accept
  data =  client.recv(124);
  p "RECEIVED: #{data}"
  client.close

  if (data == "SIGTERM")
    p "Killing container..."
    pid = Process.pid
    ppid = Process.ppid
    p "My pid = #{pid} and parent_pid = #{ppid}"
    `kill #{ppid}`
    break
  elsif (data == "SHTTPD") 
    p "Starting  httpd..."
    `/usr/sbin/httpd -k start`
  elsif (data == "THTTPD")
    p "Stopping  httpd..."
    `/usr/sbin/httpd -k stop`
  end

end

s.close
