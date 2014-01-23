#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

#define IS_FUNCTION 0x01
#define IS_ARRAY 0x02
#define IS_PTR 0x04
#define IS_WR 0x08
#define IS_INTERN 0x10




struct symbol_s
{
	int arrayArity;
	int regNum;
	char * name;
	stype type;
	char flags;
};


struct symbol_s * newSymbol(char * name, int reg, stype type);

void freeSymbol(struct symbol_s * s);

int isPtr(struct symbol_s * );
int isArray(struct symbol_s * );

#endif