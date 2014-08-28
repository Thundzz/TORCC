#include <stdlib.h>
#include "expression.h"
#include "type.h"
#include "util.h"
#include "str.h"

#define BUFSIZE 1024

/*HELLO THIS IS A MODIFICATION*/




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

struct expression_s * createComplexExpr(char * operation, char * prefixI, char * prefixF, struct expression_s* left, struct expression_s * right, int *regNum)
{
	char buffer[BUFSIZE];
	struct expression_s* s = createExpr();
	char * type;
	struct expression_s *floatingPoint, *i32;
	if (left->type == TYPE_FLOAT && right->type == TYPE_FLOAT)
	{
		s -> type = TYPE_FLOAT;
		type = (char*)"float";
		sprintf(buffer, "\t%%r%d = %s%s %s %%r%d, %%r%d \n", (*regNum), prefixF, operation, type , left->regNum, right->regNum);
		s -> code = concatString(left->code, right->code);
		s -> code = appendString(s->code, buffer);
	}
	else if (left->type == TYPE_INT && right->type == TYPE_INT)
	{
		s -> type = TYPE_INT;
		type = (char*)"i32";
		sprintf(buffer, "\t%%r%d = %s%s %s %%r%d, %%r%d \n", (*regNum), prefixI, operation, type , left->regNum, right->regNum);
		s -> code = concatString(left->code, right->code);
		s -> code = appendString(s->code, buffer);
	}
	else
	{
		if (left ->type == TYPE_FLOAT)
		{
			floatingPoint = left;
			i32 = right;
		}
		else if (right ->type == TYPE_FLOAT)
		{
			floatingPoint = right;
			i32 = left;
		}
		s -> type = TYPE_FLOAT;
		type = (char*)"float";
		struct expression_s * converted = convertExpression (TYPE_FLOAT, i32, regNum);
		sprintf(buffer, "\t%%r%d = %s%s %s %%r%d, %%r%d \n",
			(*regNum),prefixF, operation, type , converted->regNum, floatingPoint->regNum);
		s->code = concatString(floatingPoint->code, converted->code);
		s -> code = appendString(s->code , buffer);
	}
	s -> regNum = *regNum;
	(*regNum) ++ ;
	return s;
}


struct expression_s * createMulExpr(char * operation, struct expression_s* left, struct expression_s * right, int *regNum)
{
	return createComplexExpr(operation, (char *) "", (char *) "f",  left,  right, regNum);
}

struct expression_s * createComparisonExpr(char * operation, struct expression_s* left, struct expression_s * right, int *regNum)
{
	return createComplexExpr(operation, (char *) "i", (char *) "f",  left,  right, regNum);
}


char * correctPreOrPostFix (stype t)
{
	switch (t)
	{
		case TYPE_FLOAT:
		return (char *) "fp";
		break;
		case TYPE_INT:
		return (char *) "si";
		break;
		default:
		perror("");
		exit(EXIT_FAILURE);
		break;
	}
}
struct expression_s * convertExpression(stype dest, struct expression_s * expr, int * regNum)
{
	struct expression_s * s = createExpr();
	char buffer[BUFSIZE]= "";
	char * origin ;
	char * destination ;
	if(expr->type == dest )
	{
		return expr;
	}
	else
	{
		destination = correctPreOrPostFix(dest);
		origin = correctPreOrPostFix(expr->type);
	}
	sprintf(buffer, "\t%%r%d = %sto%s %s %%r%d to %s  \n",
		*regNum, origin, destination,typeToLLVM(expr->type), expr->regNum, typeToLLVM(dest) );
	s-> type = dest;
	s-> code = appendString(expr->code, buffer);
	s-> regNum = (*regNum)++;
	// perte d'un eventuel symbole : affectation impossible sur un cast.
	return s;

}

struct expression_s * createFinalExpr(struct expression_s * left, char assignOp, struct expression_s * right, int * regNum)
{
	struct expression_s *tmp = convertExpression(left->type, right, regNum);;
	struct expression_s *s = NULL;
	switch (assignOp)
	{
		case '+':
		s = createMulExpr((char *) "add", left, tmp, regNum);
		break;
		case '-':
		s = createMulExpr((char *) "sub", left, tmp, regNum);
		break;
		case '*':
		s = createMulExpr((char *) "mul", left, tmp, regNum);
		break;
		case '=':
		s= tmp;
		break;
		default:
		break;
	}
	if(assignOp != '=')
	{
		free(tmp);
	}
	/* Affectation */
	left->symbol->regNum = s->regNum;
	return s;
}


/* Vive la liberte d'expression */
void freeExpression(struct expression_s * e)
{
	free (e);
}
