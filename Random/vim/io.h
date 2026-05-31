#pragma once

#include "vim.h"

void outc(const char c);
void outs(const char* s);
void clear_screen(void);
void write_to_file(linebuf*, const char*);
