#include "hashtable.h"
#include "symbol.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	struct symbol_s s ;
	hashtable_t * myHashtable =  ht_create( 101 );
	printf("%lu \n",	ht_hash(myHashtable, (char *) "hello") );

	hashtable_t *hashtable = ht_create( 65536  -3 );

	ht_set( hashtable,(char *) "key1", &s );
	puts("a"); 
	ht_set( hashtable, (char *) "key2", &s );
	ht_set( hashtable, (char *) "key3", &s );
	ht_set( hashtable, (char *) "key4", &s );

	printf( "%p, %ld\n", ht_get( hashtable,(char *) "key2" ),ht_hash(myHashtable,(char *)"key2") );
	printf( "%p, %ld\n", ht_get( hashtable,(char *) "key1" ),ht_hash(myHashtable,(char *) "key1") );
	printf( "%p, %ld\n", ht_get( hashtable,(char *) "key3" ),ht_hash(myHashtable,(char *) "key3") );
	printf( "%p, %ld\n", ht_get( hashtable,(char *) "key4" ),ht_hash(myHashtable,(char *) "key4") );
	printf( "%ld\n",ht_hash(hashtable,(char *) "Poxdoupidou") );

	return 0;
}

