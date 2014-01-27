#include "str.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUFSIZE 1024

typedef struct String
{
	char * s;
	int bleft, bright;
	string * left;
	string * right;

} string;

int isLeaf(const string * s)
{
	return(s->s != NULL);
}


void reportStringError(char * c)
{
	fprintf(stderr, "Error in string manipulation : %s. \n", c);
	exit(EXIT_FAILURE);
}


string * allocString()
{
	string * s = malloc (sizeof(string));
	if (s == NULL)
	{
		reportStringError("memory allocation denied");
	}
	s->s = NULL;
	s->left =  s-> right = NULL;
	s->bright = s->bleft = 1;
	return s;
}

string * newString(char * c)
{
	if(c == NULL)
		return NULL;
	else
	{
		string * s =allocString();
		s->s = strdup(c);
		if (s == NULL)
		{
			reportStringError("Error at duplication");
		}
		return s;
	}
}

string * concatString(string * left,string * right)
{
	if (left == NULL)
	{
		return right;
	}
	else if (right == NULL)
	{
		return(left);
	}
	else
	{
		string * s = allocString();
		s->left = left;
		s->right = right;
		return (s);
	}
}

string * appendString(string * left, char * right)
{
	return (concatString(left, newString(right)));
}


void destroyString(string * s)
{
	if (s == NULL)
	{
		return;
	}
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
	if (str == NULL)
	{
		return;
	}
	if(isLeaf(str))
	{
		fputs(str->s, file);
	}
	else
	{
		if(str->bleft)
			printString(file, str->left );
		if(str->bright)
			printString(file, str->right );
	}
}

void hideLeftString(string *str)
{
	str->bleft = 0;
}
void hideRightString(string *str)
{
	str->bright = 0;
}

void printWholeString(FILE * file, const string * str )
{
	if (str == NULL)
	{
		return;
	}
	if(isLeaf(str))
	{
		fputs(str->s, file);
	}
	else
	{
		printWholeString(file, str->left );
		printWholeString(file, str->right );
	}
}

string * appendLabel(string * left, int label)
{
	char  buffer[BUFSIZE];
	sprintf(buffer, "\tl%d : \n", label);
	return appendString(left, buffer);
}

string * appendBranch(string * left, int regCond , int labelTrue, int labelFalse )
{
	char  buffer[BUFSIZE];
	if (regCond != 0)
	{
		sprintf(buffer , "\tbr i1 %%r%d, label %%l%d, label %%l%d\n", regCond, labelTrue, labelFalse);
		return appendString(left, buffer);
	}
	else
	{
		sprintf(buffer , "\tbr label %%l%d\n", labelTrue);
		return appendString(left, buffer);	
	}
}