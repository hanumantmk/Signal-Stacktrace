#ifndef SIGNAL_STACKTRACE_H
#define SIGNAL_STACKTRACE_H

#define _GNU_SOURCE

#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/uio.h>

#include <ucontext.h>

#define STACK_DEPTH 32

#if __WORDSIZE == 64
#define SIGNAL_STACKTRACE_IP_REG REG_RIP
#else
#define SIGNAL_STACKTRACE_IP_REG REG_EIP
#endif

#define SIGNAL_STACKTRACE_STR_AND_SIZE(str) str, strlen(str)

typedef struct signal_stacktrace_signal {
  int signal;
  int exit;
} signal_stacktrace_signal_t;

static int signal_stacktrace_actions[100];

char* signal_stacktrace_itoa(char * buf, int size, int val){
  int i = size - 2;
  buf[size - 1] = '\0';

  for(; val && i ; --i, val /= 10) {
    buf[i] = "0123456789"[val % 10];
  }

  return &buf[i+1];
}

static void signal_stacktrace_handler(int signal, siginfo_t * si, void * secret)
{
  ucontext_t * uc = secret;

  pid_t tid = syscall(SYS_gettid);
  pid_t pid = getpid();

  char tid_buf[32];
  char pid_buf[32];

  char * str_tid = signal_stacktrace_itoa(tid_buf, sizeof(tid_buf), tid);
  char * str_pid = signal_stacktrace_itoa(pid_buf, sizeof(pid_buf), pid);

  struct iovec iov[] = {
    { SIGNAL_STACKTRACE_STR_AND_SIZE("Signal Stacktrace pid=(") },
    { SIGNAL_STACKTRACE_STR_AND_SIZE(str_pid) },
    { SIGNAL_STACKTRACE_STR_AND_SIZE(") tid=(") },
    { SIGNAL_STACKTRACE_STR_AND_SIZE(str_tid) },
    { SIGNAL_STACKTRACE_STR_AND_SIZE("): \n") },
  };

  writev(1, iov, sizeof(iov) / sizeof(*iov));

  void * trace[STACK_DEPTH];

  int trace_size = backtrace(trace, STACK_DEPTH);

  trace[1] = (void *)uc->uc_mcontext.gregs[SIGNAL_STACKTRACE_IP_REG];

  backtrace_symbols_fd(trace + 1, trace_size - 1, 1);

  write(1, SIGNAL_STACKTRACE_STR_AND_SIZE("\n"));

  if (signal_stacktrace_actions[signal]) exit(signal);
}

int signal_stacktrace(signal_stacktrace_signal_t * signals)
{
  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO | SA_RESTART;
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
