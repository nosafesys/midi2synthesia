# Compiler settings
CC = gcc
CFLAGS = -Iincludes -I/usr/local/include/SDL3 -I/usr/local/include -D_REENTRANT $(shell pkg-config --cflags glib-2.0) -g
LDFLAGS = -L/usr/local/lib -lSDL3 -lSDL3_gfx -lSDL3_ttf -lportmidi -l:libsmf.a $(shell pkg-config --libs glib-2.0) -lm

# Source files and target
SOURCES = $(wildcard src/*.c)
TARGET = build/midi2synthesia

# Default target
all: $(TARGET)

# Rule to create the target
$(TARGET): $(SOURCES)
	@mkdir -p build
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -rf build

