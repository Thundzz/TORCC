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
		printString(file, str->left );
		printString(file, str->right );
	}
}
