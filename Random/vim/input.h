#pragma once

#include "vim.h"

#define EESC     '\e'
#define EBKSPC   '\177'
#define EUP      "\e[A"
#define EDOWN    "\e[B"
#define ERIGHT   "\e[C"
#define ELEFT    "\e[D"
#define ESIGTSTP 0x1A

enum direction
{
  UP,
  DOWN,
  RIGHT,
  LEFT
};

void handle_input(char);
void handle_input_normal(char);
void handle_input_insert(char);
void handle_input_command(char);
void switch_mode(enum mode md);
