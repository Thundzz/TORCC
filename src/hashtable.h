#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "symbol.h"

/* Type declaration */
typedef struct entry_s entry_t;

typedef struct hashtable_s hashtable_t;

/* Function declaration */

long ht_hash( hashtable_t *hashtable, char *key );
hashtable_t *ht_create( int size );
entry_t *ht_newpair( char *key,struct symbol_s *value );
void ht_set( hashtable_t *hashtable, char *key, struct symbol_s *value );

struct symbol_s *ht_get( hashtable_t *hashtable, char *key );
void ht_free(hashtable_t * hashtable, int freeSymbols);



#endif