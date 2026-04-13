#include <string.h>
#include <unistd.h>

#include "utility.h"

void echo(const char* arg)
{
  if (is_empty(arg)) return;

  int err;
  err = write(STDOUT_FILENO, arg, strlen(arg));
  if (err >= 0)
    write(STDOUT_FILENO, "\n", 1);
  if (err < 0)
    panic("bash: failed to write to stdout while executing echo\n", err);
}
