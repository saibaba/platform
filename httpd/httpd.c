#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#include "ruby.h"

static ID id_push;

static VALUE m_init(VALUE self, VALUE config)
{
  rb_iv_set(self, "@config", config);
  return self;
}

int create_launcher(void *arg) 
{
  int status = -1;
  pid_t pid = fork();
 
  if (pid < 0) {
    perror("fork");
  }
  else if (pid) {
    fprintf(stderr, "execvp httpd start lancher waiting...\n");
    waitpid(pid, &status, WUNTRACED | WCONTINUED); 
    fprintf(stderr, "return status for httpd -k start: %d\n", status);
  } else {
    char * argv[]  = {"httpd-tenant-1", "-k", "start", NULL};
    fprintf(stderr, "execvp httpd start\n");
    status = execvp("/usr/sbin/httpd", argv);
    perror("httpd-start");
  }

  fprintf(stderr, "as init I wait forever so my childeren do not die!\n");
  while (1) { }
  return INT2NUM(status);
}

static VALUE m_start(VALUE self)
{

  void *childstack;
  int stacksize = getpagesize() * 4;
  void *stack = malloc(stacksize);
  if (!stack) {
    perror("malloc");
    return INT2NUM(-1);
  }
  childstack = stack + stacksize;
  int flags = CLONE_NEWNS| CLONE_NEWPID;
  int cpid =  clone(create_launcher, childstack, flags, (void *) NULL);
  if (cpid < 0) {
    perror("clone");
    return INT2NUM(-1);
  }
  return INT2NUM(cpid);
}

static VALUE m_stop(VALUE self)
{
  pid_t pid = fork();
  int status = -1;

  if (pid) {
    waitpid(pid, &status, WUNTRACED | WCONTINUED); 
  } else {
    char * argv[]  = {"httpd", "-k", "stop", NULL};
    status = execvp("httpd", argv);
  }
  return INT2NUM(status);
}

VALUE cHttpd;

void Init_httpd() {

  cHttpd = rb_define_class("Httpd", rb_cObject);
  rb_define_method(cHttpd, "initialize", m_init, 1);
  rb_define_method(cHttpd, "start", m_start, 0);
  rb_define_method(cHttpd, "stop", m_stop, 0);
  id_push = rb_intern("push");
}
