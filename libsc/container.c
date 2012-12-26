#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include "container.h"

static void handler(int sig, siginfo_t *si, void *unused)
{
  printf("Got SIGTERM at address: 0x%lx\n", (long) si->si_addr);
  exit(0);
}


int wait_for_children(pid_t pid, const char *label)
{

  pid_t w;
  int status;
  fprintf(stderr, "wait_for_children: %s\n", label);

  do {
    w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
    if (w == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
      printf("exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("killed by signal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
      printf("stopped by signal %d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
      printf("continued\n");
    }
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  return status;
}

int start_container(void * arg)
{
  pid_t pid;

  printf("In the CONTAINER, my pid is: %d (must be 1, is it?)\n", getpid());
 
  pid = fork();
  printf ("fork executed with rv=%d\n", pid);
  if (pid < 0) {
    perror("fork-init");
    return pid;
  }
  if (!pid) {
    fprintf(stderr, "Child (init) started, executing initrc ...\n");
    char *args[] = {"sh", "initrc", NULL};
    execvp("/bin/sh", args);
    perror("execvp-initrc shell");
    fprintf(stderr, "THIS STEP SHOULD NEVER EXECUTE!!!!\n");
  }

  fprintf(stderr, "pid of my init child(shell) is %d\n", pid);
  fprintf(stderr, "init waiting for initrc to finish \n");
  wait_for_children(pid, "initrc executor shell");

  fprintf(stderr, "init installing signal handler for SIGTERM!\n");

  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = handler;
  if (sigaction(SIGTERM, &sa, NULL) == -1) 
    perror("sigaction");

  fprintf(stderr, "init waiting forever so children do not die!\n");
  while (1) { }
  return (0);
}
