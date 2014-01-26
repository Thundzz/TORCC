#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include "str.h"

#define IS_FUNC 0x01
#define IS_ARRAY 0x02
#define IS_PTR 0x04
#define IS_WR 0x08
#define IS_TORCSIAN 0x10




struct symbol_s
{
	int arrayArity;
	int regNum;
	char * name;
	stype type;
	char flags;
	struct symbol_list * params;
	string * paramsCode;
};

struct symbol_list
{
	struct symbol_s *symbol;
	struct symbol_list *next;
};

struct symbol_s * newSymbol(char * name, int reg, stype type, char flags);
struct symbol_list * createList(struct symbol_s *s);
void addSymbolInList(struct symbol_list * l, struct symbol_s * symbol);

void freeList(struct symbol_list * l, int freeSymbols);
void freeSymbol(struct symbol_s * s);

int isPtr(struct symbol_s * );
int isArray(struct symbol_s * );
int isWritable(struct symbol_s * );
int isFunction(struct symbol_s * );

#endif