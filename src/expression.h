#ifndef EXPRESSION_H
#define EXPRESSION_H


#include "type.h"
#include "symbol.h"
#include "str.h"

struct argument_expression_list_s
{
	struct expression_s * expr;
	struct argument_expression_list_s * next;
};

struct expression_s
{
	string * code;
	struct symbol_s * symbol;
	int isPtr;
	int regNum;
	stype type;
};


struct argument_expression_list_s * createArgExprList(struct expression_s * s);
void freeArgExprList(struct argument_expression_list_s *);

void appendArgExprList(struct argument_expression_list_s *l, struct expression_s * e );

struct expression_s * createExpr();


#endif

