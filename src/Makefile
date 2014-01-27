LEX=lex
YACC=yacc
CFLAGS=-Wall -g3
CC=gcc

all:parse

parse:y.tab.c lex.yy.c torcsVars.o hashtable.o symbol.o expression.o str.o util.o environment.o map.o statement.o
	$(CC) $(CFLAGS) -o $@ $^

compiletest: drive.o parse

y.tab.c:grammar.y 
	$(YACC) -d $^

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

teststr: teststr.o str.o
	$(CC) $(CFLAGS) -o $@ $^

hashtabletest: hashtable.o hashtabletest.o symbol.o
		$(CC) $(CFLAGS) -o $@ $^

%.o:%.ll
	llc -filetype=obj -relocation-model=pic -o $@ $^

drive.ll: parse test.c
	./parse test.c 1>drive.ll



%.o:  %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f grammar.c drive.ll scanner.c *.o *\# *~ parse lex.yy.c y.tab.* hashtabletest teststr
