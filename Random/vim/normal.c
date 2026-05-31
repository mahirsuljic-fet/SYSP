#include "input.h"
#include "io.h"
#include "term.h"
#include "vim.h"
#include <stddef.h>
#include <stdio.h>

void normal_goto_begin(void)
{
  pos.x = 0;
  outc('\r');
}

void normal_goto_end(void)
{
  pos.x = mbuf.buf[pos.y].sz;
  update_caret();
}

void normal_newline(void)
{
  pos.x = 0;
  buffer_insert_line(&mbuf, pos.y);
  update_caret();
  clear_current_line();
  for (size_t i = pos.y; i < mbuf.count; ++i)
  {
    if (mbuf.buf[i].sz > 0)
      outs(mbuf.buf[i].ptr);
    outs("\n\r");
  }
  switch_mode(INSERT);
}

void normal_newline_after(void)
{
  pos.y++;
  normal_newline();
}

void normal_newline_before(void)
{
  normal_newline();
}

void normal_up(void)
{
  if (pos.y == 0) return;
  pos.y--;
  line* line = get_current_line();
  if (pos.x >= line->sz) pos.x = line->sz;
  update_caret();
}

void normal_down(void)
{
  if (pos.y + 1 >= mbuf.count) return;
  pos.y++;
  line* line = get_current_line();
  if (pos.x >= line->sz) pos.x = line->sz;
  update_caret();
}

void normal_right(void)
{
  if (pos.x >= mbuf.buf[pos.y].sz) return;
  pos.x++;
  outs(ERIGHT);
}

void normal_left(void)
{
  if (pos.x == 0) return;
  pos.x--;
  outs(ELEFT);
}
