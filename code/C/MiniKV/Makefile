CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -pthread
INC     := -Iinclude
SRC     := $(wildcard src/*.c)
SRC_NOMAIN := $(filter-out src/main.c,$(SRC))
OBJ     := $(SRC_NOMAIN:.c=.o)

mini_kv: $(OBJ) src/main.o
	$(CC) $(CFLAGS) $(INC) -o $@ $^

TESTS := test_value test_store test_aof test_race stress
$(TESTS): %: tests/%.c $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ tests/$*.c $(OBJ)

%.o: %.c $(wildcard include/mkv/*.h)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

all: mini_kv $(TESTS)
test: $(TESTS)
	./test_value && ./test_store && ./test_aof
clean:
	rm -f $(OBJ) src/main.o mini_kv $(TESTS)
.PHONY: all test clean
