#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <execinfo.h>

#include "signal_stacktrace.h"

#define __USE_GNU
#include <ucontext.h>

#define STACK_DEPTH 32

int signal_stacktrace_actions[100];

static void sig_handler(int signal, siginfo_t * si, void * secret)
{
  ucontext_t * uc = secret;

  void * trace[STACK_DEPTH];

  int trace_size = backtrace(trace, STACK_DEPTH);

  trace[1] = (void *)uc->uc_mcontext.gregs[REG_EIP];

  backtrace_symbols_fd(trace + 1, trace_size - 1, 1);

  if (signal_stacktrace_actions[signal]) exit(signal);
}

int signal_stacktrace(signal_stacktrace_signal_t * signals)
{
  memset(signal_stacktrace_actions, 0, 100);

  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = &sig_handler;

  signal_stacktrace_signal_t * ptr;

  for (ptr = signals; ptr->signal; ptr++) {
    signal_stacktrace_actions[ptr->signal] = ptr->exit;
    sigaction(ptr->signal, &sa, NULL);
  }

  return 0;
}
