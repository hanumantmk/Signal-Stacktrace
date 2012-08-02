#ifndef SIGNAL_STACKTRACE_H
#define SIGNAL_STACKTRACE_H

#define _GNU_SOURCE

#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

#include <ucontext.h>

#define STACK_DEPTH 32

#if __WORDSIZE == 64
#define SIGNAL_STACKTRACE_IP_REG REG_RIP
#else
#define SIGNAL_STACKTRACE_IP_REG REG_EIP
#endif


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

  trace[1] = (void *)uc->uc_mcontext.gregs[SIGNAL_STACKTRACE_IP_REG];

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
