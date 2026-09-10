CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2
SRC     := main.c lexer.c token.c parser.c ast.c type_check.c \
           opcode.c chunk.c codegen.c value.c vm.c errors.c

mycc: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -lm -o $@

clean:
	rm -f mycc

.PHONY: clean
