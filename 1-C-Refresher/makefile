# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable name
TARGET = stringFun

# Default target
all: $(TARGET)

# Compile source to executable
$(TARGET): stringFun.c
	$(CC) $(CFLAGS) -o $(TARGET) $^

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean