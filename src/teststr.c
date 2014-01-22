#include "str.h"


#include <stdio.h>




int main(int argc, char const *argv[])
{
	string * hello = newString("hello");
	string * world = newString(" world");
	string * helloWorld = concatString(hello, world);
	string * vide = newString(NULL);
	string * empty = newString(NULL);

	string * coucou = appendString(helloWorld, "!\n" );
	printString(stdout, hello);
	printString(stdout, coucou);

	string * total = concatString(coucou, vide);
	string * total2 = concatString(empty, total);


	destroyString(total2);



	return 0;
}
