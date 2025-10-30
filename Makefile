CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11 -Wno-format-truncation -Wno-unused-parameter -Wno-unused-function

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = build/main

INCLUDE_DIRS = -Iinclude -Ilib -Ilib/jansson

JANNSON_DIR = lib/jansson
JANSSON_SRC = $(wildcard $(JANNSON_DIR)/*.c)
JANSSON_OBJ = $(JANSSON_SRC:lib/jansson/%.c=build/jansson/%.o)

CFLAGS += $(INCLUDE_DIRS)

all: $(TARGET)

$(TARGET): $(OBJ) $(JANSSON_OBJ)
	$(CC) $(OBJ) $(JANSSON_OBJ) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/jansson/%.o: lib/jansson/%.c | build/jansson
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

build/jansson:
	mkdir -p build/jansson

clean:
	rm -rf build

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean