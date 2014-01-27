#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "type.h"

char error[1024];

int yyerror (char *s);
char * strFusion (char * str, char * toAppend)
  {/*
    int newSize = strlen(str) + strlen (toAppend) +1  ;
    str =  realloc (str, newSize) ;
    strncat ( str, toAppend, strlen(toAppend));
    free(toAppend);

    return (str);*/
    return strdup("");
}

char * typeToLLVM (stype CType)
{
	switch(CType){
		case TYPE_INT:
		return (char *) "i32";
		break;
		case TYPE_VOID:
		return (char *) "void";
		break;
		case TYPE_FLOAT:
		return (char *) "float";
		break;
		default:
		sprintf(error, "wrong type : %d  \n", CType);
		yyerror (error);
		exit(EXIT_FAILURE);
	}
}

char * typeToCType (stype CType)
{
	switch(CType){
		case TYPE_INT:
		return (char *) "int";
		break;
		case TYPE_VOID:
		return (char *) "void";
		break;
		case TYPE_FLOAT:
		return (char *) "float";
		break;
		default:
		sprintf(error, "wrong type : %d  \n", CType);
		yyerror (error);
		exit(EXIT_FAILURE);
	}
}