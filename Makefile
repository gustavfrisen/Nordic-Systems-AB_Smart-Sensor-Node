CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = build/main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean