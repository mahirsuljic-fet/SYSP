#include "utility.h"

#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

bool is_whitespace(const char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v'; }

bool is_empty(const char* str)
{
  while (*str)
  {
    if (!is_whitespace(*str)) return false;
    str++;
  }
  return true;
}

#define GREEN "\033[1;32m"
#define RESET "\033[0m"

void show_prompt(const char* user, size_t user_len, const char* prompt, size_t prompt_len)
{
  write(STDOUT_FILENO, GREEN, sizeof(GREEN));
  write(STDOUT_FILENO, user, user_len);
  write(STDOUT_FILENO, RESET, sizeof(RESET));
  write(STDOUT_FILENO, prompt, prompt_len);
}

#undef GREEN
#undef RESET

command parse_cmd(const char* cmd)
{
  if (strcmp(cmd, "touch") == 0) return touch;
  if (strcmp(cmd, "echo") == 0) return echo;
  if (strcmp(cmd, "cat") == 0) return cat;
  if (strcmp(cmd, "ls") == 0) return ls;
  return NULL;
}

char get_input(char buffer[], size_t buffer_size)
{
  memset(buffer, '\0', buffer_size);
  char* arg_ptr = buffer;
  int err;

  bool skipped_whitespace = false;
  while ((err = read(STDIN_FILENO, arg_ptr, 1)) > 0)
  {
    const char c = *arg_ptr;
    if (!skipped_whitespace && (c == ' ' || c == '\t'))
      continue;

    skipped_whitespace = true;

    if (c == ' ' || c == '\t' || c == '\n')
    {
      *arg_ptr = '\0';
      return c;
    }

    if (arg_ptr < buffer + buffer_size)
    {
      arg_ptr++;
      continue;
    }

    return c;
  }

  return err;
}

char get_inode_type(unsigned char type)
{
  switch (type)
  {
  case DT_SOCK: return 's';
  case DT_REG: return '-';
  case DT_BLK: return 'b';
  case DT_CHR: return 'c';
  case DT_DIR: return 'd';
  case DT_LNK: return 'l';
  case DT_FIFO: return 'p';
  }
  return '?';
}

void print_dir(int dir_fd, struct dirent* entry)
{
  struct stat file_stat;

  // stat(entry->d_name, &file_stat); // ne radi za relativne putanje
  if (fstatat(dir_fd, entry->d_name, &file_stat, 0) < 0)
  {
    char msg[282];
    snprintf(msg, sizeof(msg), "ls: failed to fstat file %s\n", entry->d_name);
    cmd_err(msg);
  }

  char inode[25];
  char nlink[35];
  char uid[15];
  char gid[15];
  char size[25];
  char mtime[25];
  char type;

  type = get_inode_type(entry->d_type);
  snprintf(inode, sizeof(inode), "%lu ", entry->d_ino);
  snprintf(nlink, sizeof(nlink), "%2lu ", file_stat.st_nlink);
  snprintf(uid, sizeof(uid), "%4u ", file_stat.st_uid);
  snprintf(gid, sizeof(gid), "%4u ", file_stat.st_gid);
  snprintf(size, sizeof(size), "%ld", file_stat.st_size);
  strftime(mtime, sizeof(mtime), "\t%Y-%m-%d %H:%M ", localtime(&file_stat.st_mtim.tv_sec));

  int err = 0;
  if (err >= 0)
    err = write(STDOUT_FILENO, inode, strlen(inode));
  if (err >= 0)
    err = write(STDOUT_FILENO, &type, 1);
  if (err >= 0)
    err = write(STDOUT_FILENO, nlink, strlen(nlink));
  if (err >= 0)
    err = write(STDOUT_FILENO, uid, strlen(uid));
  if (err >= 0)
    err = write(STDOUT_FILENO, gid, strlen(gid));
  if (err >= 0)
    err = write(STDOUT_FILENO, size, strlen(size));
  if (err >= 0)
    err = write(STDOUT_FILENO, mtime, strlen(mtime));
  if (err >= 0)
    err = write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
  if (err >= 0)
    err = write(STDOUT_FILENO, "\n", 1);

  if (err < 0)
    panic("bash: failed to write to stdout while executing ls\n", err);
}

void panic(const char* msg, int code)
{
  write(STDERR_FILENO, msg, strlen(msg));
  exit(code);
}

void cmd_err(const char* msg)
{
  int err;
  if ((err = write(STDERR_FILENO, msg, strlen(msg))) < 0)
    panic("bash: failed to write to stderr\n", err);
}
