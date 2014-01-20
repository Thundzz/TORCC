#ifndef STR_H
#define STR_H


#include <stdio.h>

typedef struct String string;


/* Function declaration */


string * newString(const char * c);
string * concatString(const string * left,const string * right);
string * appendString(const string * left,const char * right);

void destroyString(string * s);
void printString(FILE * file, const string * str );


#endif


