require './syscall'

#previous_handler = trap('TERM') { raise RuntimeError, "*************** signal in ruby *************" }

`ip link add v0-tenant-1 type veth peer name v1-tenant-1`
`ifconfig v0-tenant-1 10.0.0.101 up`
puts Syscall.new.newpid

