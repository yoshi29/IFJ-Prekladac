# Autor: Filip Vágner, xvagne08

PROJ=ifj20

CC=gcc
STD=-std=c99
CFLAGS=-Wall -Wextra

.PHONY: all
all:
	$(CC) $(STD) $(CFLAGS) -o $(PROJ) error.c generator.c isusedlist.c main.c parser.c psa.c psa_stack.c scanner.c str.c symtable.c

.PHONY: clean
clean:
	rm -f $(PROJ) *.o *.out