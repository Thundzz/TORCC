#include "str.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


typedef struct String
{
	char * s;
	string * left;
	string * right;

} string;

int isLeaf(const string * s)
{
	return(s->s != NULL);
}


void reportError(char * c)
{
	fprintf(stderr, "Error in string manipulation : %s. \n", c);
	exit(EXIT_FAILURE);
}


string * allocString()
{
	string * s = malloc (sizeof(string));
	if (s == NULL)
	{
		reportError("memory allocation denied");
	}
	s->s = NULL;
	s->left =  s-> right = NULL;
	return s;
}

string * newString(const char * c)
{
	string * s =allocString();
	s->s = strdup(c);
	if (s == NULL)
	{
		reportError("Error at duplication");
	}
	return s;
}

string * concatString(const string * left,const string * right)
{
	string * s = allocString();
	s->left = left;
	s->right = right;
	return (s);
}

string * appendString(const string * left,const char * right)
{
	return (concatString(left, newString(right)));
}


void destroyString(string * s)
{
	if(isLeaf(s))
	{
		free(s->s);
	}
	else
	{
		destroyString(s->left);
		destroyString(s->right);
	}
	free(s);
}

void printString(FILE * file, const string * str )
{
	if(isLeaf(str))
	{
	  fputs(str->s, file);
	}
	else
	{
		printString(file, str->left );
		printString(file, str->right );
	}
}
