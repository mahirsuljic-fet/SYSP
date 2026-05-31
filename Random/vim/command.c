#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "input.h"
#include "io.h"
#include "term.h"
#include "vim.h"

void command_bkspc(void)
{
  if (cmd_buf_ptr > cmd_buf)
  {
    cmd_buf_ptr--;
    *cmd_buf_ptr = '\0';
    outs(ELEFT);
    outc(' ');
    outs(ELEFT);
  }
}

void command_echo(char c)
{
  if (cmd_buf_ptr < cmd_buf + CMD_BUFF_SIZE)
  {
    *cmd_buf_ptr = c;
    cmd_buf_ptr++;
    outc(c);
  }
}

void command_quit(void)
{
  exit(0);
}

void command_write(void)
{
  write_to_file(&mbuf, current_filepath);
  switch_mode(NORMAL);
}

void command_clear_buffer(void)
{
  memset(cmd_buf, 0, CMD_BUFF_SIZE);
  cmd_buf_ptr = cmd_buf;
}

void command_parse_fail(void)
{
  clear_current_line();
  outs("\rUnknown command!");
  command_clear_buffer();
  switch_mode(NORMAL);
}

void command_parse(void)
{
  if (strcmp(cmd_buf, "q") == 0)
    command_quit();
  else if (strcmp(cmd_buf, "w") == 0)
    command_write();
  else if (cmd_buf[0] == 'e' && cmd_buf[1] == ' ')
    command_open_file(command_get_single_arg());
  else if (strcmp(cmd_buf, "wq") == 0)
  {
    command_write();
    command_quit();
  }
  else
    command_parse_fail();
}

char* command_get_single_arg(void)
{
  char* ptr = cmd_buf;
  while (*ptr != ' ' && ptr < cmd_buf + CMD_BUFF_SIZE) ptr++;
  return ptr + 1;
}

void command_open_file(char* new_filepath)
{
  strcpy(current_filepath, new_filepath);
  command_clear_buffer();
  switch_mode(NORMAL);
}
