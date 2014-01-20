LEX=lex
YACC=yacc
CFLAGS=-Wall -g3
CC=gcc

all:parse

parse:y.tab.c lex.yy.c map.o torcsVars.o
	$(CC) $(CFLAGS) -o $@ $^

y.tab.c:grammar.y 
	$(YACC) -d $^

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

teststr: teststr.o str.o
	$(CC) $(CFLAGS) -o $@ $^

hashtabletest: hashtable.o hashtabletest.o
		$(CC) $(CFLAGS) -o $@ $^

teststr.o: teststr.c str.o
	$(CC) $(CFLAGS) -c teststr.c	

map.o: map.c map.h
	$(CC) $(CFLAGS) -c map.c

torcsVars.o: torcsVars.c torcsVars.h
	$(CC) $(CFLAGS) -c torcsVars.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c


%.o:  %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f grammar.c scanner.c *.o *~ parse lex.yy.c y.tab.* hashtabletest teststr
