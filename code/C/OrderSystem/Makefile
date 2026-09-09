CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -pthread
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)

all: orderapp bench

orderapp: $(OBJ) main.o
	$(CC) $(CFLAGS) -o $@ $^

bench: $(OBJ) benchmark.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(wildcard src/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main.o benchmark.o orderapp bench

.PHONY: all clean
