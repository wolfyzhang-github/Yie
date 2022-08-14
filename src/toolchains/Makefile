CC=gcc
CFLAGS=-Wall -O1 -g -fcommon
LCFLAGS=-O1 -fcommon
LEX = flex
YACC=bison
LEXLIB = -lfl

all: yie


# These are the explicit rules for making yie.

isa.o: lib/isa.c lib/isa.h
	$(CC) $(CFLAGS) -c lib/isa.c

yie.o: yie.c lib/isa.h
	$(CC) $(CFLAGS) -c yie.c

yie: yie.o isa.o
	$(CC) $(CFLAGS) yie.o isa.o -o bin/yie
	rm yie.o isa.o

.PHONY: clean
clean:
	rm -f *.o *.yo *.exe yas-grammar.c lex.yy.c
	rm -rf bin/yab bin/yie
