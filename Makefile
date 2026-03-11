CC ?= cc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -O2

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET := poc_tool

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

test: all
	./tests/smoke_test.sh

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all test clean
