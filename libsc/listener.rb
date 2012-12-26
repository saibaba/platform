require 'socket'

def termrc
  IO.popen(['/bin/sh', 'termrc', :err => [:child, :out]]) { |x|
    p x.read
  }
end

s = TCPServer.new('', 5555)
 
while true do
  client = s.accept
  data =  client.recv(124);
  p "RECEIVED: #{data}"
  client.close

  if (data == "SIGTERM")
    p "Killing container..."
    termrc
    pid = Process.pid
    ppid = Process.ppid
    p "My pid = #{pid} and parent_pid = #{ppid}"
    Process.kill("TERM", ppid)
    break
  elsif (data == "SIGUSR1")
    p "SIGUSR1 container..."
    termrc
    pid = Process.pid
    ppid = Process.ppid
    p "My pid = #{pid} and parent_pid = #{ppid}"
    Process.kill("USR1", ppid)
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
