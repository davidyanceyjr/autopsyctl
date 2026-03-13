CC ?= cc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -O2

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET := poc_tool
TEST_TARGET := tests/collector_test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_TARGET): tests/collector_test.c src/collect_proc_status.c include/poc.h
	$(CC) $(CFLAGS) -Iinclude tests/collector_test.c src/collect_proc_status.c -o $@

test: all $(TEST_TARGET)
	./$(TEST_TARGET)
	./tests/smoke_test.sh

clean:
	rm -f src/*.o $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
