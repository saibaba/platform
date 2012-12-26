#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

int main(char *argv[])
{
  fprintf(stderr, "********************  exec'ing httpd start ...\n");
  char *args[] = {"httpd", "-k", "start", NULL};
  int rv = execvp("/usr/sbin/httpd", args);
}
