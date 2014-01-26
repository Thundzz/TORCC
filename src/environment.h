#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


#include "hashtable.h"
#include "symbol.h"

struct environment {
	hashtable_t * table;
	struct environment * englobant;
};




struct symbol_s * getSymbolFromEnv(struct environment * env, char * key);


struct environment * createEnvironment ();

struct environment * englober(struct environment * newenv, struct environment * used);

struct environment * deglober(struct environment * env);

void setSymbol(struct environment * env, struct symbol_s * sym, char * key);

void freeEnvironment(struct environment * env, int freeSymbols);

#endif