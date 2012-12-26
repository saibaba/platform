#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#include "container.h"

#include "ruby.h"

static ID id_push;

static VALUE m_init(VALUE self)
{
  return self;
}

static VALUE m_pivot_root(VALUE self, VALUE new_root, VALUE old_root)
{
  const char *p = StringValueCStr(new_root);
  const char *o = StringValueCStr(old_root);
  int rv = pivot_root(p, o);
  printf("new_root=%s\nold_root=%s\nrv=%d\n", p, o, rv);
  return INT2NUM(rv);
}

static VALUE m_chroot(VALUE self, VALUE new_root)
{
  char *p = StringValueCStr(new_root);
  int rv = chroot(p);
  printf("chroot rv=%d\n", rv);
  return INT2NUM(rv);
}

static VALUE m_create_namespace(VALUE self)
{
  int rv = unshare(CLONE_NEWNET|CLONE_FS);
  printf("unshare rv=%d\n", rv);
  return INT2NUM(rv);
}

static VALUE m_newns(VALUE self)
{
  int rv = unshare(CLONE_NEWNS);
  printf("unshare rv=%d\n", rv);
  if (rv < 0) perror("newns");
  return INT2NUM(rv);
}

static VALUE m_newnet(VALUE self)
{
  int rv = unshare(CLONE_NEWNET);
  printf("unshare rv=%d\n", rv);
  if (rv < 0) perror("newnet");
  return INT2NUM(rv);
}

static VALUE m_start_container(VALUE self)
{

  void *childstack;
  int stacksize = getpagesize() * 4;
  void *stack = malloc(stacksize);
  if (!stack) {
    perror("malloc");
    return INT2NUM(-1);
  }
  childstack = stack + stacksize;
  int flags = CLONE_NEWNET| CLONE_NEWNS| CLONE_NEWPID;
  int cpid =  clone(start_container, childstack, flags, (void *) self);
  if (cpid < 0) {
    perror("clone");
    return INT2NUM(-1);
  }

  fprintf(stderr, "PID of init from outside: %d\n", cpid);

  char cmd[1000];
  sprintf(cmd, "ip link set v1-tenant-1 netns %d", cpid);
  printf("init linking veth to child \n");
  int rv = system(cmd);
  if (rv < 0) perror("system");
  //fprintf(stderr, "parent sleeping 10secs\n");
  //sleep(10);
  int ret = 0;

  return INT2NUM(ret);
}

static VALUE m_test_start_container(VALUE self)
{
   start_container(NULL);

  return INT2NUM(0);
}

VALUE cSyscall;

void Init_syscall() {

  cSyscall = rb_define_class("Syscall", rb_cObject);
  rb_define_method(cSyscall, "initialize", m_init, 0);
  rb_define_method(cSyscall, "pivot_root", m_pivot_root, 2);
  rb_define_method(cSyscall, "chroot", m_chroot, 1);
  rb_define_method(cSyscall, "create_namespace", m_create_namespace, 0);
  rb_define_method(cSyscall, "newns", m_newns, 0);
  rb_define_method(cSyscall, "start_container", m_start_container, 0);
  rb_define_method(cSyscall, "newnet", m_newnet, 0);
  rb_define_method(cSyscall, "test_start_container", m_test_start_container, 0);
  id_push = rb_intern("push");
}
