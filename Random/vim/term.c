#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "io.h"
#include "term.h"
#include "vim.h"

struct termios saved_termios;

void save_term(struct termios* term)
{
  tcgetattr(STDIN_FILENO, term);
}

void load_term(struct termios* term)
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, term);
}

void enter_alt_buffer(void)
{
  outs("\033[?1049h");
}

void exit_alt_buffer(void)
{
  outs("\033[?1049l");
}

void reset_term(void)
{
  exit_alt_buffer();
  load_term(&saved_termios);
}

void setup_term(void)
{
  struct termios raw_mode = saved_termios;
  raw_mode.c_lflag &= ~(ICANON | ISIG | ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
  enter_alt_buffer();
}

void realloc_line(line* line)
{
  line->cap *= 2;
  line->ptr = realloc(line->ptr, line->cap);
  line->ptr[line->sz] = '\0';
}

void buffer_insert_line(linebuf* mbuf, size_t index)
{
  if (index > mbuf->count) err(EXIT_FAILURE, "Attempt to insert at index greater than buffer line count");

  mbuf->count++;
  mbuf->buf = reallocarray(mbuf->buf, mbuf->count, sizeof(line));
  memmove(mbuf->buf + index + 1, mbuf->buf + index, (mbuf->count - index - 1) * sizeof(line));

  static const size_t default_size = 0;
  static const size_t default_capacity = 1;

  line* line = &mbuf->buf[index];
  line->sz = default_size;
  line->cap = default_capacity;
  line->ptr = malloc(default_capacity);
  memset(line->ptr, '\0', line->cap);
}

void buffer_build(linebuf* mbuf)
{
  mbuf->count = 0;
  mbuf->buf = malloc(mbuf->count * sizeof(line));
  buffer_insert_line(mbuf, 0);
}

void clear_buffer(linebuf* mbuf)
{
  for (size_t i = 0; i < mbuf->count; ++i)
    free(mbuf->buf[i].ptr);
  free(mbuf->buf);
}

void update_caret() { set_caret(pos.x, pos.y); }

void set_caret(unsigned int x, unsigned int y)
{
  char str[32];
  sprintf(str, "\e[%u;%uH", y + 1, x + 1);
  outs(str);
}

void clear_current_line(void)
{
  outs("\e[2K");
}

line* get_current_line(void)
{
  return &mbuf.buf[pos.y];
}

struct winsize get_winsize(void)
{
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  return ws;
}
