#ifndef EXPRESSION_H
#define EXPRESSION_H


#include "type.h"
#include "symbol.h"
#include "str.h"

struct expression_s
{
	string * code;
	int isConstant;
	int regNum;
	int isSymbol;
	int intConstVal;
	stype type;
	float floatConstVal;
};

void initExpr(struct expression_s * s);

struct expression_s * createExpr();


#endif