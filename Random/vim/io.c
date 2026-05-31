#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "io.h"
#include "vim.h"

void outc(const char c)
{
  int e;
  while ((e = write(STDOUT_FILENO, &c, sizeof(c))) == 0);
  if (e < 0)
    err(e, "Failed to write to standard output.");
}

void outs(const char* s)
{
  const size_t s_len = strlen(s);
  size_t written = 0;
  ssize_t e;

  while (written < s_len)
  {
    e = write(STDOUT_FILENO, s + written, s_len - written);
    if (e <= 0) break;
    written += (size_t)e;
  }

  if (e < 0)
    err(e, "Failed to write to standard output.");
}

void clear_screen(void)
{
  outs("\033[2J");
  outs("\033[1;1H");
}

void write_to_file(linebuf* buf, const char* filepath)
{
  if (!buf || !filepath) return;
  int fd = open(filepath, O_WRONLY | O_TRUNC | O_CREAT, 0666);
  if (fd < 0) err(EXIT_FAILURE, "Failed to open file");
  for (size_t i = 0; i < mbuf.count; ++i)
  {
    line* line = &mbuf.buf[i];
    if (line->sz == 0) continue;
    write(fd, line->ptr, line->sz);
    if (i != mbuf.count - 1)
      write(fd, "\n", 1);
  }
  close(fd);
}
