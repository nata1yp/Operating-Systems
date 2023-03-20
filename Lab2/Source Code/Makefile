.PHONY: all clean

all: ask2-fork prog1.2 prog1.3  prog1.4

CC = gcc
CFLAGS = -g -Wall -O2
SHELL= /bin/bash

prog1.2: prog1.2.o proc-common.o tree.o
	$(CC) $(CFLAGS) $^ -o $@

prog1.4: prog1.4.o proc-common.o tree.o 
	$(CC) $(CFLAGS) $^ -o $@

prog1.3: prog1.3.o proc-common.o tree.o
	$(CC) $(CFLAGS) $^ -o $@

ask2-fork: ask2-fork.o proc-common.o
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.i: %.c
	gcc -Wall -E $< | indent -kr > $@

clean: 
	rm -f *.o pstree-this ask2-fork prog1.2 prog1.3 prog1.4
