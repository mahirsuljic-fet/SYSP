#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

#define BUFF_SIZE 128

typedef int (*command)();

static const char* arg_delims = " \t";

command load_cmd(char*);
command parse_cmd(const char*);
const char* get_next_arg();

int print_dirent(DIR*, struct dirent*);

int touch();
int echo();
int cat();
int ls();

int main()
{
  const char prompt[] = "\033[1;31m>\033[0m ";

  char* line = NULL;
  size_t size = 0;

  command cmd;

  while (1)
  {
    write(STDOUT_FILENO, prompt, sizeof(prompt));
    if (getdelim(&line, &size, '\n', stdin) <= 0) break;

    cmd = load_cmd(line);

    if (cmd == NULL)
    {
      char msg[] = "Invalid command!\n";
      write(STDOUT_FILENO, msg, sizeof(msg));
      continue;
    }

    cmd();
  }

  write(STDOUT_FILENO, "\n", 1);

  return EXIT_SUCCESS;
}

command load_cmd(char* line)
{
  char* cmd_str = strtok(line, arg_delims);
  char* last_cmd_char = cmd_str + (strlen(cmd_str) - 1);
  if (*last_cmd_char == '\n') *last_cmd_char = '\0';
  return parse_cmd(cmd_str);
}

const char* get_next_arg()
{
  char* arg = strtok(NULL, arg_delims);

  if (arg)
  {
    char* last_arg_char = arg + (strlen(arg) - 1);
    if (*last_arg_char == '\n') *last_arg_char = '\0';
  }

  return arg;
}

command parse_cmd(const char* str)
{
  if (!strcmp(str, "touch")) return touch;
  if (!strcmp(str, "echo")) return echo;
  if (!strcmp(str, "cat")) return cat;
  if (!strcmp(str, "ls")) return ls;
  return NULL;
}

int touch()
{
  const char* arg;
  while ((arg = get_next_arg()))
  {
    if (!access(arg, F_OK))
      return utime(arg, NULL);

    const size_t arg_len = strlen(arg);
    const char* arg_end = arg + arg_len;

    char input[arg_len];
    strcpy(input, arg);

    char* walk = input;
    while ((walk = strchr(walk, '/')) || walk >= arg_end)
    {
      *walk = '\0';
      mkdir(input, 0755);
      *walk = '/';
      ++walk;
    }

    creat(arg, 0644);
  }

  return 0;
}

int echo()
{
  const char* arg;
  while ((arg = get_next_arg()))
  {
    write(STDOUT_FILENO, arg, strlen(arg));
    write(STDOUT_FILENO, " ", 1);
  }
  write(STDOUT_FILENO, "\n", 1);
  return 0;
}

int cat()
{
  const size_t block_size = 512;
  char block[block_size];

  const char* arg;
  while ((arg = get_next_arg()))
  {
    printf("arg: %s\n", arg);

    int fd = open(arg, O_RDONLY);

    if (fd < 0)
    {
      const char msg[] = "cat: couldn't open file or it doesn't exist\n";
      write(STDOUT_FILENO, msg, sizeof(msg));
      return fd;
    }

    struct stat stat;
    fstat(fd, &stat);

    if (S_ISDIR(stat.st_mode))
    {
      const char msg[] = "cat: given path is a directory\n";
      write(STDOUT_FILENO, msg, sizeof(msg));
      close(fd);
      return -1;
    }

    size_t read_len;
    while ((read_len = read(fd, block, block_size)))
      write(STDOUT_FILENO, block, read_len);

    close(fd);
  }

  return 0;
}

int ls()
{
  bool first = true;
  const char* arg = NULL;
  while ((arg = get_next_arg()) || first)
  {
    if (first && !arg)
      arg = ".";

    first = false;

    DIR* dir;
    if ((dir = opendir(arg)) == NULL)
    {
      const char msg[] = "ls: couldn't open directory\n";
      write(STDOUT_FILENO, msg, sizeof(msg));
      return -1;
    }

    int err;
    struct dirent* entry;
    while ((entry = readdir(dir)))
      if ((err = print_dirent(dir, entry)))
        return err;
  }

  return 0;
}

int print_dirent(DIR* dir, struct dirent* entry)
{
  struct stat stat;

  int err;
  if ((err = fstatat(dirfd(dir), entry->d_name, &stat, 0)))
  {
    const char msg[] = "ls: failed to stat entry\n";
    write(STDOUT_FILENO, msg, sizeof(msg));
    return err;
  }

  char type_;
  switch (entry->d_type)
  {
  case DT_REG: type_ = '-'; break;
  case DT_DIR: type_ = 'd'; break;
  case DT_LNK: type_ = 'l'; break;
  case DT_BLK: type_ = 'b'; break;
  case DT_CHR: type_ = 'c'; break;
  case DT_WHT: type_ = 'w'; break;
  case DT_FIFO: type_ = 'f'; break;
  case DT_SOCK: type_ = 's'; break;
  case DT_UNKNOWN:
  default: type_ = '?';
  }

  char inode[25];
  char nlink[35];
  char uid[15];
  char gid[15];
  char size[25];
  char mtime[25];
  char type[3];

  snprintf(inode, sizeof(inode), "%ld ", stat.st_ino);
  snprintf(type, sizeof(type), "%c ", type_);
  snprintf(nlink, sizeof(nlink), "%ld ", stat.st_nlink);
  snprintf(uid, sizeof(uid), "%u ", stat.st_uid);
  snprintf(gid, sizeof(gid), "%u ", stat.st_gid);
  snprintf(size, sizeof(size), "%ld\t", stat.st_size);
  strftime(mtime, sizeof(mtime), "%Y-%m-%d %H:%M ", localtime(&stat.st_mtim.tv_sec));

  write(STDOUT_FILENO, inode, strlen(inode));
  write(STDOUT_FILENO, type, strlen(type));
  write(STDOUT_FILENO, nlink, strlen(nlink));
  write(STDOUT_FILENO, uid, strlen(uid));
  write(STDOUT_FILENO, gid, strlen(gid));
  write(STDOUT_FILENO, size, strlen(size));
  write(STDOUT_FILENO, mtime, strlen(mtime));
  write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
  write(STDOUT_FILENO, "\n", 1);

  return 0;
}
