#include <stdlib.h>
#include "expression.h"
#include "type.h"


void initExpr(struct expression_s * s)
{
	s->isConstant = 0;
	s->isSymbol = 0;
	s-> intConstVal = 0;
	s->type = TYPE_UNDEF;
	s->floatConstVal = 0.0;
	s->regNum = 0;
	s->code = newString(NULL);
}


struct expression_s * createExpr()
{

	struct expression_s * s;
	s = malloc(sizeof(struct expression_s));
	initExpr(s);
	return s;

}