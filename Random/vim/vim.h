#pragma once

#include <stddef.h>

#define CMD_BUFF_SIZE     100
#define FILEPATH_MAX_SIZE 50

// line in buffer
typedef struct
{
    char* ptr;  // pointer to the line data
    size_t sz;  // current size of the line
    size_t cap; // current capacity of the line
} line;

// buffer of lines
typedef struct
{
    line* buf;    // line array
    size_t count; // number of lines
} linebuf;

typedef struct
{
    size_t x;
    size_t y;
} position;

enum mode
{
  NORMAL,
  INSERT,
  COMMAND
};

extern linebuf mbuf;
extern position pos;
extern char cmd_buf[CMD_BUFF_SIZE];
extern char* cmd_buf_ptr;
extern char current_filepath[FILEPATH_MAX_SIZE];
extern enum mode mode;
