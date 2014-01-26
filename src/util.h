#ifndef UTIL_H
#define UTIL_H

#define DONTFREESYMBOLS 0
#define PLEASEFREESYMBOLS 1

#include "type.h"
char * strFusion (char * str, char * toAppend);

char * typeToLLVM (stype CType);


char * typeToCType (stype CType);


#endif
