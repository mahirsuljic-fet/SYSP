#include "utility.h"

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>

#define DEFAULT_ARG "."

void ls(const char* arg)
{
  if (is_empty(arg)) arg = DEFAULT_ARG;

  DIR* dir = opendir(arg);

  if (!dir)
  {
    char msg[100];
    snprintf(msg, sizeof(msg), "ls: couldn't open directory %s\n", arg);
    cmd_err(msg);
    return;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)))
    print_dir(dirfd(dir), entry);

  int err;
  if ((err = closedir(dir)) < 0)
  {
    char msg[150];
    snprintf(msg, sizeof(msg), "bash: failed to close directory %s while executing ls\n", arg);
    panic(msg, err);
  }
}

#undef DEFAULT_ARG
