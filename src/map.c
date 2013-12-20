#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"



/* A minimalistic map implementation provided by D.Bonnin and edited by us*/

struct var
{
  int val, type, mode;
};

static char * var_names[MAX_VAR];
static struct var vars[MAX_VAR];

/* static int var_vals[MAX_VAR]; */
/* static int var_types[MAX_VAR]; */
/* static int var_mode[MAX_VAR]; */

static int known_var=0;

void var_init(struct var *v)
{
    v->val = VAL_UNDEF;
    v->type = TYPE_UNDEF;
    v->mode = MODE_RDWR;
}
void map_init(){
  int i;
  known_var = 0;
  for(i=0; i<MAX_VAR; i++){
    var_names[i] = NULL;
    var_init(&vars[i]);
  }
}

void map_end(){
  int i;
  for(i=0; i<known_var; i++){
    free(var_names[i]);
  }
}

int get_position(char * x){
  int f = 0, l=known_var-1, m=0, comp;
  int found = 0;
  while(!found && f<=l){
    m = (f+l)/2;
    comp = strcmp(x, var_names[m]);
    if(comp < 0){
      f = m+1;
    }
    else if(comp > 0){
      l = m-1;
    }
    else{
      found = 1;
    }
  }
  if(found)
    return m;
  else
    return f;
}

struct var * map_get(char * x)
{
  int pos = get_position(x);
  if(pos < known_var && strcmp(var_names[pos], x) == 0)
    return &vars[pos];
  else
    return NULL;
}

int map_get_val(char * x){
  struct var *v = map_get(x);
  if(v == NULL)
    return VAL_UNDEF;
  else return v->val;
}

int map_get_type(char * x){
  struct var *v = map_get(x);
  if(v == NULL)
    return TYPE_UNDEF;
  else return v->type;
}

int map_get_mode(char * x){
  struct var *v = map_get(x);
  if(v == NULL)
    return TYPE_UNDEF;
  else return v->mode;
}

struct var * map_getOrCreateVar(char *x)
{
   int pos = get_position(x);
   if(var_names[pos] != NULL && strcmp(x, var_names[pos]) == 0){ // value already in the table
     return &vars[pos];
   }
   else{
     if(known_var < MAX_VAR){
       int i;
       for(i=known_var; i>pos; i--){
	 var_names[i] = var_names[i-1];
	 vars[i] = vars[i-1]; 
       }
       var_names[pos] = strdup(x);
       known_var++;
       var_init(&vars[pos]);
       return &vars[pos];
     }
     else {
       fprintf(stderr, "Too many variables\n");
       exit(1);
     }      
   }
}

void map_set_val(char * x, int val){
  struct var * v = map_getOrCreateVar(x);
  v->val = val;
}

void map_set_type(char * x, int type){
  struct var * v = map_getOrCreateVar(x);
  v->type = type;
}

void map_set_mode(char * x, int mode){
  struct var * v = map_getOrCreateVar(x);
  v->mode = mode;
}

