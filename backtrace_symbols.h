#include <stdio.h>

char **backtrace_symbols(void *const *buffer, int size);
char * backtrace_symbols_str(char * prefix);
void backtrace_symbols_stream(FILE * stream, char * prefix);
