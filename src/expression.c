#include <stdlib.h>
#include "expression.h"
#include "type.h"


void initExpr(struct expression_s * s)
{
	s->symbol = NULL;
	s->type = TYPE_UNDEF;
	s->regNum = 0;
	s->code = newString(NULL);
	s->isPtr = 0;
}


struct expression_s * createExpr()
{
	struct expression_s * s;
	s = (struct expression_s *) malloc(sizeof(struct expression_s));
	initExpr(s);
	return s;

}

struct argument_expression_list_s * createArgExprList(struct expression_s * s)
{
	struct argument_expression_list_s  * a;
	a = (struct argument_expression_list_s *) malloc(sizeof(struct argument_expression_list_s));
	a-> expr = s;
	a-> next = NULL;
	return a;
}

void freeArgExprList(struct argument_expression_list_s * l)
{
	if (l == NULL)
		return;

	freeArgExprList(l->next);
	free(l);
}


void appendArgExprList(struct argument_expression_list_s * l, struct expression_s * e )
{
	if (l == NULL)
	{
		// Pas possible
	}
	if (l->next== NULL)
	{
		l->next = createArgExprList(e);  
	}
	else
	{
		appendArgExprList(l-> next, e);
	}
}