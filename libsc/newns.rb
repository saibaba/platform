require './syscall'

puts Syscall.new.newns

exec '/bin/sh'

