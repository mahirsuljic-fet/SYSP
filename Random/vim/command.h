#pragma once

void command_echo(const char);
void command_bkspc(void);
void command_parse(void);
void command_quit(void);
void command_write(void);
void command_open_file(char*);
void command_parse_fail(void);
char* command_get_single_arg(void);
