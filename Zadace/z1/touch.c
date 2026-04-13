#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include "utility.h"

#define DEFAULT_FILE_PERMS 0644
#define DEFAULT_DIR_PERMS  0755

void touch(const char* path)
{
  if (is_empty(path)) return;

  // provjera da li fajl postoji
  if (access(path, F_OK) == 0)
  {
    if (utime(path, NULL) < 0)
    {
      char msg[100];
      snprintf(msg, sizeof(msg), "touch: failed to change access time for file %s\n", path);
      cmd_err(msg);
    }
    return;
  }

  // provjera parent direktorija
  size_t path_size = 0;
  bool has_parent_dirs = false;
  while (path[path_size] != '\0')
  {
    if (path[path_size] == '/') has_parent_dirs = true;
    ++path_size;
  }

  // kreiranje parent direktorija
  if (has_parent_dirs)
  {
    // kopija kako bi parametar path mogao ostati const
    char path_copy[path_size];
    strcpy(path_copy, path);

    size_t slash_index = 0;
    while (slash_index < path_size)
    {
      while (path_copy[slash_index] != '/' && slash_index < path_size) ++slash_index;
      if (slash_index >= path_size) break;
      path_copy[slash_index] = '\0';
      if (access(path_copy, F_OK | W_OK))
      {
        if (mkdir(path_copy, DEFAULT_DIR_PERMS) < 0)
        {
          char msg[100];
          snprintf(msg, sizeof(msg), "touch: failed to create the directory %s\n", path_copy);
          cmd_err(msg);
        }
      }
      path_copy[slash_index] = '/';
      slash_index++;
    }
  }

  // kreairanje fajla
  if (access(path, W_OK))
  {
    if (creat(path, DEFAULT_FILE_PERMS) < 0)
    {
      char msg[100];
      snprintf(msg, sizeof(msg), "touch: failed to create the file %s\n", path);
      cmd_err(msg);
    }
    return;
  }
}

#undef DEFAULT_FILE_PERMS
#undef DEFAULT_DIR_PERMS
