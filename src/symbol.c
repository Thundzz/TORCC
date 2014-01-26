#include "symbol.h"
#include "stdlib.h"
#include "string.h"
#include "type.h"
#include "util.h"


struct symbol_s * newSymbol(char * name, int reg, stype type, char flags)
{
	struct symbol_s * sym = (struct symbol_s *) malloc(sizeof(struct symbol_s));
	sym->name = strdup(name);
	sym->type = type;
	sym->paramsCode = NULL;
	sym->regNum = reg;
	sym->flags = flags;
	sym->arrayArity = 0;
	sym->params = NULL;
	return(sym);
}

int isPtr(struct symbol_s * s)
{
	return (int) (s->flags & (IS_PTR | IS_ARRAY));
}

int isArray(struct symbol_s * s)
{
	return (s->flags &  IS_ARRAY);
}

int isWritable(struct symbol_s * s)
{
	return (s->flags &  IS_WR);	
}

int isFunction(struct symbol_s * s)
{
	return (s->flags &  IS_FUNC);	
}

void freeSymbol(struct symbol_s * s)
{
	if (NULL != s->name)
	{
		free (s->name);
	}
	free(s);
}

struct symbol_list * initList(struct symbol_list *l, struct symbol_s * symbol)
{
	l->symbol = symbol;
	l->next = NULL;
	return l;
}

struct symbol_list * createList(struct symbol_s *s)
{
	return initList((struct symbol_list *) malloc(sizeof(struct symbol_list)), s);
}

void addSymbolInList(struct symbol_list * l, struct symbol_s * symbol)
{
	if(l->next == NULL)
		l->next = createList(symbol);
	else
		addSymbolInList(l->next, symbol);
}

void freeList(struct symbol_list * l, int freeSymbols)
{
	if(l == NULL)
		return;
	if (freeSymbols)
	{
		freeSymbol(l->symbol);
	}
	freeList(l->next, freeSymbols);
	free(l);
}