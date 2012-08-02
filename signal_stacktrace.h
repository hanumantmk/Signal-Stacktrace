#include <signal.h>

typedef struct signal_stacktrace_signal {
  int signal;
  int exit;
} signal_stacktrace_signal_t;

int signal_stacktrace(signal_stacktrace_signal_t * signals);
