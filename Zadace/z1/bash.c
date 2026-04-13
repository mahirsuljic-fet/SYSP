#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utility.h"

#define BUFF_SIZE 64

int main()
{
  const char* user = getenv("USER");
  const char prompt[] = "$ ";
  const size_t user_len = strlen(user);
  const size_t prompt_len = strlen(prompt);

  char cmd_buf[BUFF_SIZE];
  char arg_buf[BUFF_SIZE];
  int err = 0;

  while (!err)
  {
    show_prompt(user, user_len, prompt, prompt_len);

    // dohvatanje komande
    int cmd_ret = get_input(cmd_buf, sizeof(cmd_buf));

    if (cmd_ret == '\0')
      break;

    if (cmd_ret < 0)
    {
      err = cmd_ret;
      break;
    }

    // parsiranje komande
    command cmd = parse_cmd(cmd_buf);

    if (!cmd)
    {
      char msg[BUFF_SIZE + 24];
      snprintf(msg, sizeof(msg), "invalid command: %s\n", cmd_buf);
      cmd_err(msg);
      continue;
    }

    // ako nema argumenata, koristi se prazan argument
    if (cmd_ret == '\n')
    {
      cmd("");
      continue;
    }

    // procesiranje argumenata
    int arg_ret;
    while ((arg_ret = get_input(arg_buf, sizeof(arg_buf))) && arg_ret > 0)
    {
      cmd(arg_buf);
      if (arg_ret == '\n') break;
    }

    // sanity check
    if (arg_ret == '\0')
      break;

    if (arg_ret < 0)
    {
      err = arg_ret;
      break;
    }
  }

  write(STDOUT_FILENO, "\n", 1);
}
