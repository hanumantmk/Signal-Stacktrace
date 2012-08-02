#include "signal_stacktrace.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void usage(char * prog)
{
    printf("Usage - %s [\"segv\" or \"loop\"]\n", prog);
    exit(1);
}

void loop(void)
{
  while (1) {
    sleep(1);
  }
}

void baz(void)
{
  char * g = (char *)0xDEADBEEF;

  *g = 10;
}

void bar(void (* cb)(void))
{
  cb();
}

void foo(void (* cb)(void))
{
  bar(cb);
}

int main(int argc, char ** argv)
{
  if (argc != 2) usage(argv[0]);

  signal_stacktrace_signal_t signals[] = {
    { SIGSEGV, 1 },
    { SIGUSR1, 0 },
    { 0 },
  };

  signal_stacktrace(signals);

  if (strcmp(argv[1], "segv") == 0) {
    foo(&baz);
  } else if (strcmp(argv[1], "loop") == 0) {
    printf("pid: %d\n", getpid());
    foo(&loop);
  } else {
    usage(argv[0]);
  }

  return 0;
}

