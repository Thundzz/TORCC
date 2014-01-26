#ifndef	STATEMENT_H
#define STATEMENT_H
#include "str.h"
#include "type.h"

struct statement
{
	stype returnType;
	string * code;
	int returns;
};

struct statement_list
{
	string *code;
	int returns;
	stype returnType;
	int lastStatementReturns;
};

struct statement * createStatement ();
struct statement_list * createStatementList ();

void freeStatement(struct statement *s);
void freeStatementList(struct statement_list *l);


#endif