CC=gcc
CFLAGS=-Wall -O1 -g -fcommon

all: yie

yie: yie.o
	$(CC) $(CFLAGS) yie.o -o bin/yie
	rm yie.o

yie.o: yie.c yie.h
	$(CC) $(CFLAGS) -c yie.c

.PHONY: clean
clean:
	rm -f *.o bin/yie
