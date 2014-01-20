#ifndef HASHTABLE_H
#define HASHTABLE_H

/* Type declaration */
typedef struct entry_s entry_t;

typedef struct hashtable_s hashtable_t;


/* Function declaration */

long ht_hash( hashtable_t *hashtable, char *key );
hashtable_t *ht_create( int size );
entry_t *ht_newpair( char *key, char *value );
void ht_set( hashtable_t *hashtable, char *key, char *value );

char *ht_get( hashtable_t *hashtable, char *key );



#endif