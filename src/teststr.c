#include "str.h"


#include <stdio.h>




int main(int argc, char const *argv[])
{
	string * hello = newString("hello");
	string * world = newString(" world");
	string * helloWorld = concatString(hello, world);

	string * coucou = appendString(helloWorld, "!\n" );
	printString(stdout, hello);
	printString(stdout, coucou);
	destroyString(coucou);



	return 0;
}
