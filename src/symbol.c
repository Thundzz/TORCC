#include "symbol.h"
#include "stdlib.h"
#include "string.h"
#include "type.h"



struct symbol_s * newSymbol(char * name, int reg, stype type)
{
	struct symbol_s * sym = (struct symbol_s *) malloc(sizeof(struct symbol_s));
	sym->name = strdup(name);
	sym->type = type;
	sym->regNum = reg;
	sym->flags = 0;
	sym->arrayArity = 0;
	return(sym);
}

int isPtr(struct symbol_s * s)
{
	return (s->flags & IS_PTR);
}

int isArray(struct symbol_s * s)
{
	return (s->flags &  IS_ARRAY);
}

void freeSymbol(struct symbol_s * s)
{
	if (NULL != s->name)
	{
		free (s->name);
	}
	free(s);
}