#pragma once

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "vim.h"

void save_term(struct termios*);
void load_term(struct termios*);
void enter_alt_buffer(void);
void exit_alt_buffer(void);
void reset_term(void);
void setup_term(void);
void buffer_build(linebuf*);
void clear_buffer(linebuf*);
void update_caret(void);
void set_caret(unsigned int x, unsigned int y);
void buffer_insert_line(linebuf* mbuf, size_t index);
void clear_current_line(void);
void realloc_linebuf(linebuf*);
void realloc_line(line*);
line* get_current_line(void);
struct winsize get_winsize(void);

extern struct termios saved_termios;
