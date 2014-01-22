#include "hashtable.h"
#include "symbol.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	symbol_t s ;
	hashtable_t * myHashtable =  ht_create( 101 );
	printf("%lu \n",	ht_hash(myHashtable, "hello") );

	hashtable_t *hashtable = ht_create( 65536  -3 );

	ht_set( hashtable, "key1", &s );
	puts("a");
	ht_set( hashtable, "key2", &s );
	ht_set( hashtable, "key3", &s );
	ht_set( hashtable, "key4", &s );

	printf( "%p, %ld\n", ht_get( hashtable, "key2" ),ht_hash(myHashtable, "key2") );
	printf( "%p, %ld\n", ht_get( hashtable, "key1" ),ht_hash(myHashtable, "key1") );
	printf( "%p, %ld\n", ht_get( hashtable, "key3" ),ht_hash(myHashtable, "key3") );
	printf( "%p, %ld\n", ht_get( hashtable, "key4" ),ht_hash(myHashtable, "key4") );
	printf( "%ld\n",ht_hash(hashtable, "Poxdoupidou") );

	return 0;
}

