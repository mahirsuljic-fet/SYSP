#include "utility.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#define BUFF_SIZE 512

void cat(const char* arg)
{
  if (is_empty(arg)) return;

  char buffer[BUFF_SIZE];
  int fd = open(arg, O_RDONLY);

  if (fd < 0)
  {
    char msg[100];
    snprintf(msg, sizeof(msg), "cat: failed to open file %s\n", arg);
    cmd_err(msg);
    return;
  }

  int err;
  while ((err = read(fd, buffer, sizeof(buffer))) > 0)
  {
    if (write(STDOUT_FILENO, buffer, err) < 0)
      panic("bash: failed to write to stdout while executing cat\n", err);
  }

  if (err < 0)
  {
    char msg[100];
    snprintf(msg, sizeof(msg), "cat: read from file failed with error code %d\n", err);
    cmd_err(msg);
  }

  if ((err = close(fd)) < 0)
  {
    char msg[100];
    snprintf(msg, sizeof(msg), "bash: failed to close file %s while executing cat\n", arg);
    panic(msg, err);
  }
}

#undef BUFF_SIZE
