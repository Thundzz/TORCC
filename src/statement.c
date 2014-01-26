#include "statement.h"
#include <malloc.h>
#include "type.h"

struct statement * createStatement ()
{
	struct statement * s = (struct statement *) malloc(sizeof(struct statement));
	s->code = NULL;
	s->returns = 0;
	s->returnType = TYPE_VOID;
	return s;
}
struct statement_list * createStatementList ()
{
	struct statement_list * l = (struct statement_list *) malloc(sizeof(struct statement_list));
	l->code = NULL;
	l->returns = 0;
	l->returnType = TYPE_VOID;
	l->lastStatementReturns = 0;
	return l;
}

void freeStatement(struct statement *s)
{
	free(s);
}

void freeStatementList(struct statement_list *l)
{
	free(l);
}