#include <string.h>
#include <sys/ioctl.h>

#include "input.h"
#include "io.h"
#include "term.h"
#include "vim.h"

void insert_bkspc(void)
{
  if (pos.x == 0) return;
  line* line = get_current_line();
  char* target = line->ptr + pos.x - 1;
  memmove(target, target + 1, line->sz - pos.x + 1);
  line->sz--;
  pos.x--;
  outs(ELEFT);
  outs(target);
  outc(' ');
  update_caret();
}

void insert_echo(char c)
{
  line* line = get_current_line();
  if (line->sz >= line->cap - 1) realloc_line(line);
  char* target = line->ptr + pos.x;
  memmove(target + 1, target, line->sz - pos.x + 1);
  *target = c;
  line->sz++;
  pos.x++;
  outs(target);
  update_caret();
}
