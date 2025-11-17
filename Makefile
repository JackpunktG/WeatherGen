CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

# Include paths
INCLUDES = -Ilib/arena_memory -Ilib/SDL2 -Isrc/objects

# Source files
SRCS = src/main.c \
       src/objects/floatingtext.c \
	   src/objects/weathergen.c \
       lib/arena_memory/arena_memory.c \
       lib/SDL2/SDL2lib.c

# Object files (in same directories as source)
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET ?= WeatherGen_Demo

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean built files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
