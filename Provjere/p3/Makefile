# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g

# Target executable name
TARGET = minesweeper

# Automatically find all .c files and convert to .o
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

run: all
	./$(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
