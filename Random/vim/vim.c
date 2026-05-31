#include <err.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "input.h"
#include "io.h"
#include "signal.h"
#include "term.h"
#include "vim.h"

linebuf mbuf;
position pos;

char cmd_buf[CMD_BUFF_SIZE];
char* cmd_buf_ptr = cmd_buf;

enum mode mode = NORMAL;

char current_filepath[FILEPATH_MAX_SIZE] = "test.txt";

static void tty_check(void);
static void init(void);
static void work_loop(void);
static void cleanup(void);

int main()
{
  tty_check();
  init();
  work_loop();
  cleanup();
  return EXIT_SUCCESS;
}

static void tty_check(void)
{
  if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
    err(EXIT_FAILURE, "Standard input or output is not the terminal.");
}

static void register_sighandlers(void)
{
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = sighandler_sigcont;
  sigaction(SIGCONT, &sa, NULL);
}

static void init(void)
{
  save_term(&saved_termios);
  atexit(reset_term);
  buffer_build(&mbuf);
  setup_term();
  register_sighandlers();
  clear_screen();
  switch_mode(NORMAL);
}

static void work_loop(void)
{
  char c;
  while (read(STDIN_FILENO, &c, sizeof(c)) == 1)
    handle_input(c);
}

static void cleanup(void)
{
  clear_buffer(&mbuf);
}
