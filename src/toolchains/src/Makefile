CC=gcc
CFLAGS=-Wall -O1 -g -fcommon
LCFLAGS=-O1 -fcommon
LEX = flex
YACC=bison
LEXLIB = -lfl
YAS=./yas

all: yis yas

# These are implicit rules for making .yo files from .ys files.
# E.g., make sum.yo
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys

# These are the explicit rules for making yis yas and hcl2c and hcl2v
yas-grammar.o: yas-grammar.c
	$(CC) $(LCFLAGS) -c yas-grammar.c

yas-grammar.c: yas-grammar.lex
	$(LEX) yas-grammar.lex
	mv lex.yy.c yas-grammar.c

isa.o: isa.c isa.h
	$(CC) $(CFLAGS) -c isa.c

yas.o: yas.c yas.h isa.h
	$(CC) $(CFLAGS) -c yas.c

yas: yas.o yas-grammar.o isa.o
	$(CC) $(CFLAGS) yas-grammar.o yas.o isa.o ${LEXLIB} -o ../output/yas

yis.o: yis.c isa.h
	$(CC) $(CFLAGS) -c yis.c

yis: yis.o isa.o
	$(CC) $(CFLAGS) yis.o isa.o -o ../output/yis

clean:
	rm -f *.o *.yo *.exe yas-grammar.c
	rm -rf ../output/*


