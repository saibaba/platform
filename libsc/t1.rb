require './syscall'

s = Syscall.new

p "pwd before = " + Dir.pwd
p s.create_namespace
Dir.chdir("/tenants/tenant1"); 
p "pwd after cr ns and chdir = " + Dir.pwd
p "pr rv = " + s.pivot_root(".", "old-root").to_s
p s.chroot(".")

__END__

  printf("Before dir...\n");
  walk("/");
  create_namespace("a");
  chdir("/tenants/tenant1"); 
  pivot_root(".", "old-root");
  chroot(".");
  printf("After dir... \n");
  walk("/");
  return 0;

