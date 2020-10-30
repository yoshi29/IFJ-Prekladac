# Autor: Filip Vágner, xvagne08

PROJ=ifj20

CC=gcc
STD=-std=c99
CFLAGS=-Wall -Wextra #-Werror -pedantic

.PHONY: all
all:
	$(CC) $(STD) $(CFLAGS) -o $(PROJ) *.c

.PHONY: clean
clean:
	rm -f $(PROJ) *.o *.out