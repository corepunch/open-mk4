#ifndef MK4_CLI_H
#define MK4_CLI_H
#include "binary.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

static inline const char *cli_value(int argc, char **argv, int *index) {
  if (*index + 1 >= argc)
    fatal("Missing value for %s", argv[*index]);
  return argv[++*index];
}

static inline int cli_int(const char *text) {
  char *end;
  errno = 0;
  long n = strtol(text, &end, 10);
  if (errno || end == text || *end || n < INT_MIN || n > INT_MAX)
    fatal("Invalid number: %s", text);
  return (int)n;
}

static inline double cli_double(const char *text) {
  char *end;
  errno = 0;
  double n = strtod(text, &end);
  if (errno || end == text || *end || !isfinite(n))
    fatal("Invalid number: %s", text);
  return n;
}
#endif
