#ifndef STR_H
#define STR_H


#include <stdio.h>

typedef struct String string;


/* Function declaration */


string * newString(char * c);
string * concatString(string * left, string * right);
string * appendString(string * left, char * right);

void destroyString(string * s);
void printString(FILE * file, const string * str );


#endif


