#ifndef SIGNAL_STACKTRACE_H
#define SIGNAL_STACKTRACE_H

#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

#ifdef __USE_GNU
#define SIGNAL_STACKTRACE_HAD_USE_GNU
#endif

#define __USE_GNU
#include <ucontext.h>

#ifndef SIGNAL_STACKTRACE_HAD_USE_GNU
#undef __USE_GNU
#endif

#define STACK_DEPTH 32

typedef struct signal_stacktrace_signal {
  int signal;
  int exit;
} signal_stacktrace_signal_t;

static int signal_stacktrace_actions[100];

static void signal_stacktrace_handler(int signal, siginfo_t * si, void * secret)
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
  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = &signal_stacktrace_handler;

  signal_stacktrace_signal_t * ptr;

  for (ptr = signals; ptr->signal; ptr++) {
    signal_stacktrace_actions[ptr->signal] = ptr->exit;
    sigaction(ptr->signal, &sa, NULL);
  }

  return 0;
}

#endif
