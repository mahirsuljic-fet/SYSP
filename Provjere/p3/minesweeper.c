#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

struct position
{
    size_t x;
    size_t y;
};

enum direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

#define MINE_COUNT 10

#define STR_UP     "\033[A"
#define STR_DOWN   "\033[B"
#define STR_RIGHT  "\033[C"
#define STR_LEFT   "\033[D"

static size_t move_count;
static struct winsize ws;
static struct termios saved_state;
static struct position player_pos;
static struct position mine_pos[MINE_COUNT];

void save_state(void);
void reset_state(void);
void enable_raw_mode(void);
void outc(char c);
void outs(const char* s);
void clear_screen();
void set_caret(unsigned short x, unsigned short y);
void handle_input(char c);
void move(enum direction);
void init_ws(void);
void init_player(void);
void init_mines(void);
void draw_mines(void);
void check_collision(void);
void draw_move_count(void);
void end_game(void);
void win_game(void);

int main()
{
  if (!isatty(STDIN_FILENO))
    err(EXIT_FAILURE, "Standard input is not a terminal");

  srand(time(NULL));
  save_state();
  atexit(reset_state);
  enable_raw_mode();
  clear_screen();
  init_ws();
  init_mines();
  draw_mines();
  init_player();

  char c;
  while (read(STDIN_FILENO, &c, sizeof(c)) > 0)
    handle_input(c);

  return EXIT_SUCCESS;
}

void init_ws(void)
{
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
}

void init_player(void)
{
  set_caret(ws.ws_col / 2, ws.ws_row / 2);
  player_pos.x = ws.ws_col / 2;
  player_pos.y = ws.ws_row / 2;
  move_count = 0;
  draw_move_count();
}

void init_mines(void)
{
  for (size_t i = 0; i < MINE_COUNT; ++i)
  {
    mine_pos[i].x = rand() % ws.ws_col;
    mine_pos[i].y = rand() % ws.ws_row;
  }
}

void draw_mines(void)
{
  for (size_t i = 0; i < MINE_COUNT; ++i)
  {
    set_caret(mine_pos[i].x, mine_pos[i].y);
    outs(STR_LEFT);
    outc('@');
  }
}

void save_state()
{
  tcgetattr(STDIN_FILENO, &saved_state);
}

void reset_state()
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_state);
}

void enable_raw_mode()
{
  struct termios raw_mode = saved_state;
  raw_mode.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
}

void outc(char c)
{
  write(STDOUT_FILENO, &c, sizeof(char));
}

void outs(const char* s)
{
  size_t len = strlen(s);
  write(STDOUT_FILENO, s, len);
}

void clear_screen() { outs("\033[2J"); }

void set_caret(unsigned short x, unsigned short y)
{
  char str[30];
  sprintf(str, "\033[%u;%uH", y, x);
  outs(str);
}

void handle_input(char c)
{
  switch (c)
  {
  case 'q': exit(0);
  case 'w': move(UP); return;
  case 's': move(DOWN); return;
  case 'a': move(LEFT); return;
  case 'd': move(RIGHT); return;
  }
}

void move(enum direction dir)
{
  switch (dir)
  {
  case UP:
    if (player_pos.y <= 1) return;
    outs(STR_UP);
    player_pos.y--;
    break;
  case DOWN:
    if (player_pos.y > ws.ws_row) return;
    outs(STR_DOWN);
    player_pos.y++;
    break;
  case RIGHT:
    if (player_pos.y > ws.ws_col) return;
    outs(STR_RIGHT);
    player_pos.x++;
    break;
  case LEFT:
    if (player_pos.x <= 1) return;
    outs(STR_LEFT);
    player_pos.x--;
    break;
  }

  ++move_count;
  if (move_count >= 100)
    win_game();

  draw_move_count();
  check_collision();
}

void check_collision(void)
{
  for (size_t i = 0; i < MINE_COUNT; ++i)
    if (player_pos.x + 1 == mine_pos[i].x && player_pos.y == mine_pos[i].y)
      end_game();
}

void draw_move_count(void)
{
  char str[30];
  set_caret(1, 1);
  sprintf(str, "Moves: %lu\n", move_count);
  outs(str);
  set_caret(player_pos.x, player_pos.y);
}

void end_game(void)
{
  const char msg[] = "KRAJ!";
  clear_screen();
  set_caret(ws.ws_col / 2 + sizeof(msg) / 2, ws.ws_row / 2);
  outs(msg);
  exit(0);
}

void win_game(void)
{
  const char msg[] = "POBJEDA!";
  clear_screen();
  set_caret(ws.ws_col / 2 - 3, ws.ws_row / 2);
  outs(msg);
  exit(0);
}
