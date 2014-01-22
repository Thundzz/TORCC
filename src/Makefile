LEX=lex
YACC=yacc
CFLAGS=-Wall -g3
CC=gcc

all:parse

parse:y.tab.c lex.yy.c map.o torcsVars.o hashtable.o symbol.o expression.o str.o
	$(CC) $(CFLAGS) -o $@ $^

y.tab.c:grammar.y 
	$(YACC) -d $^

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

teststr: teststr.o str.o
	$(CC) $(CFLAGS) -o $@ $^

hashtabletest: hashtable.o hashtabletest.o symbol.o
		$(CC) $(CFLAGS) -o $@ $^

%.o:  %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f grammar.c scanner.c *.o *\# *~ parse lex.yy.c y.tab.* hashtabletest teststr
