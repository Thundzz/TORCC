#ifndef MAP_H
#define MAP_H

/* Constants Declaration */

#define MAX_VAR 1000

#define VAL_UNDEF 0

typedef enum {TYPE_UNDEF, TYPE_INT, TYPE_FLOAT, TYPE_VOID} idType;

typedef enum {MODE_RDONLY, MODE_RDWR} idMode;


/* Function prototypes */

void map_init() ;

void map_end();


int map_get_val(char * variable);

int map_get_type(char * variable);

int map_get_mode(char * variable);

void map_set_val(char * variable, int value);

void map_set_type(char * variable, int type);

void map_set_mode(char * variable, int mode);

#endif


