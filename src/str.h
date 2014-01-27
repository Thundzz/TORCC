#ifndef STR_H
#define STR_H


#include <stdio.h>

typedef struct String string;


/* Function declaration */


string * newString(char * c);
string * concatString(string * left, string * right);
string * appendString(string * left, char * right);


string * appendBranch(string * left, int regCond , int labelTrue, int labelFalse );
string * appendLabel(string * left, int label);

void destroyString(string * s);
void hideRightString(string *str);
void hideLeftString(string *str);
void printString(FILE * file, const string * str );
void printWholeString(FILE * file, const string * str );


#endif


