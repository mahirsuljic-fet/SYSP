#include <asm-generic/ioctls.h>
#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "command.h"
#include "input.h"
#include "insert.h"
#include "io.h"
#include "normal.h"
#include "term.h"
#include "vim.h"

void handle_input(char c)
{
  switch (mode)
  {
  case NORMAL: handle_input_normal(c); break;
  case INSERT: handle_input_insert(c); break;
  case COMMAND: handle_input_command(c); break;
  }
}

void goto_background(void)
{
  reset_term();
  raise(SIGTSTP);
}

void switch_mode(enum mode md)
{
  char* mode_str = NULL;

  switch (md)
  {
  case NORMAL:
    memset(cmd_buf, 0, CMD_BUFF_SIZE);
    cmd_buf_ptr = cmd_buf;
    mode_str = "NORMAL";
    break;
  case INSERT:
    mode_str = "INSERT";
    break;
  case COMMAND:
    mode_str = ":";
    break;
  }

  struct winsize ws = get_winsize();
  set_caret(0, ws.ws_row - 1);
  clear_current_line();
  outs(mode_str);

  if (md != COMMAND)
    update_caret();

  mode = md;
}

void handle_input_normal(char c)
{
  switch (c)
  {
  case 0x1A: goto_background(); break;
  case '0': normal_goto_begin(); break;
  case '$': normal_goto_end(); break;
  case 'k': normal_up(); break;
  case 'j': normal_down(); break;
  case 'l': normal_right(); break;
  case 'h': normal_left(); break;
  case 'o': normal_newline_after(); break;
  case 'O': normal_newline_before(); break;
  case 'i': switch_mode(INSERT); break;
  case ':':
  case ';':
    switch_mode(COMMAND);
    break;
  }
}

void handle_input_insert(char c)
{
  switch (c)
  {
  case EESC:
    switch_mode(NORMAL);
    break;
  case EBKSPC:
    insert_bkspc();
    break;
  default:
    insert_echo(c);
    break;
  }
}

void handle_input_command(char c)
{
  switch (c)
  {
  case EESC:
    switch_mode(NORMAL);
    return;
  case EBKSPC:
    command_bkspc();
    return;
  case '\n':
    command_parse();
    return;
  default:
    command_echo(c);
    return;
  }
}
