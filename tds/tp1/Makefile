all: a.out

LLVM:
	llvm-as ex.ll -f -o ex.bc
	llvm-ld -o=ex -native ex.bc

lex.yy.c: ex.l
	lex ex.l

y.tab.c: ex.y
	yacc ex.y -d

a.out: y.tab.c lex.yy.c
	gcc -g -Wall y.tab.c lex.yy.c -ll

clean:
	rm -f *~ *.c *.h *.bc
