CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)
BIN     := minidb

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c $(wildcard src/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	mkdir -p data && ./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: run clean
