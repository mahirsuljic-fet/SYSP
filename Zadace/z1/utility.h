#pragma once

#include "commands.h"

#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

typedef void (*command)(const char*);

bool is_empty(const char*);
char get_input(char[], size_t);
command parse_cmd(const char*);
bool is_whitespace(const char);
void print_dir(int, struct dirent*);
void panic(const char*, int);
void cmd_err(const char*);
void show_prompt(const char*, size_t, const char*, size_t);
