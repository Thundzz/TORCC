#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux. See feature_test_macros(7) */


#include "hashtable.h"
#include "symbol.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <string.h>

int yyerror (char *s);


struct entry_s {
	char *key;
	struct symbol_s *value;
	struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;	
};

typedef struct hashtable_s hashtable_t;


/* Create a new hashtable. */
hashtable_t *ht_create( int size ) {
	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 ) return NULL;

/* Allocate the table itself. */
	if( ( hashtable =  malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->size = size;

	return hashtable;	
}



/* Hash a string for a particular hash table. */

long ht_hash( hashtable_t *hashtable, char *key )
{
	int i, j;
	int lengthAsInt = strlen(key)/sizeof(int);
	char * reste;
	unsigned long hash = 0;
	for (i = 0; i < lengthAsInt*sizeof(int) ; i+= sizeof(int))
	{
		unsigned long mult = 1;
		for (j = 0; j < sizeof(int); j++)
		{
			//printf("%d, %d, %c, mult = %lu, hash = %lu \n", i, j, key[i+j], mult, hash);
			hash += key[i+j]* mult;
			mult *= 256;
		}
	}
	reste = key + lengthAsInt*sizeof(int);
	unsigned long mult = 1;
	for (i = 0; i < strlen(reste); ++i)
	{
		hash += reste[i]* mult;
		mult = mult << 8;
	}
	return ((long) (hash % hashtable->size));
}




/* Create a key-value pair. */
entry_t *ht_newpair( char *key, struct symbol_s *value ) {
	entry_t *newpair;

	if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->value = value ) == NULL ) {
		return NULL;
	}

	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( hashtable_t *hashtable, char *key, struct symbol_s *value ) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		last = next;
		next = next->next;
	}

/* There's already a pair. Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
		//TODO yyerror ("redefinition");
		free( next->value );
		next->value = value;

/* Nope, could't find it. Time to grow a pair. */
	} else {
		newpair = ht_newpair( key, value );

/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
/* We're at the end of the linked list in this bin. */
		} else if ( next == NULL ) {
			last->next = newpair;
/* We're in the middle of the list. */
		} else {
			newpair->next = next;
			last->next = newpair;
		}
	}
}

/* Retrieve a key-value pair from a hash table. */
struct symbol_s *ht_get( hashtable_t *hashtable, char *key ) {
	int bin = 0;
	entry_t *pair;

	bin = ht_hash( hashtable, key );

/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
		pair = pair->next;
	}

/* Did we actually find anything? */
	if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
		return NULL;

	} else {
		return pair->value;
	}
}

void ht_entry_free(entry_t * e, int freeSymbols)
{
	if(e == NULL)
		return;
	else
	{
		ht_entry_free(e->next, freeSymbols);
	}
	if (freeSymbols)
	{
		freeSymbol(e->value);
	}
	free(e->key);
	free(e);
}

void ht_free(hashtable_t * hashtable, int freeSymbols)
{
	int i;
	for (i = 0; i < hashtable-> size; ++i)
	{
		ht_entry_free(hashtable->table[i], freeSymbols);
	}
	free (hashtable->table);
	free(hashtable);
}

/* Tests
int main( int argc, char **argv ) {

	hashtable_t *hashtable = ht_create( 65536 );

	ht_set( hashtable, "key1", "inky" );
	ht_set( hashtable, "key2", "pinky" );
	ht_set( hashtable, "key3", "blinky" );
	ht_set( hashtable, "key4", "floyd" );

	printf( "%s\n", ht_get( hashtable, "key1" ) );
	printf( "%s\n", ht_get( hashtable, "key2" ) );
	printf( "%s\n", ht_get( hashtable, "key3" ) );
	printf( "%s\n", ht_get( hashtable, "key4" ) );

	return 0;
}

*/