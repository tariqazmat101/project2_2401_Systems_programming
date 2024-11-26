# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Target executable name
TARGET = simulation

# Source files
SRCS = main.c manager.c event.c resource.c system.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .c files into .o files
%.o: %.c defs.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)
