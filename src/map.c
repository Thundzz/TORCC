#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

/* A minimalistic map implementation provided by D.Bonnin and edited by us*/

static char * var_names[MAX_VAR];
static int var_vals[MAX_VAR];
static int var_types[MAX_VAR];
static int var_mode[MAX_VAR];

static int known_var=0;

void map_init(){
  int i;
  known_var = 0;
  for(i=0; i<MAX_VAR; i++){
    var_names[i] = NULL;
    var_vals[i] = VAL_UNDEF;
    var_types[i] = TYPE_UNDEF;
    var_mode[i] = MODE_RDWR;
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


int map_get_val(char * x){
  int pos = get_position(x);
  if(pos < known_var && strcmp(var_names[pos], x) == 0)
    return var_vals[pos];
  else
    return 0;
}



int map_get_type(char * x){
  int pos = get_position(x);
  if(pos < known_var && strcmp(var_names[pos], x) == 0)
    return var_types[pos];
  else
    return TYPE_UNDEF ;
}

void map_set_val(char * x, int v){
  int pos = get_position(x);
  if(var_names[pos] != NULL && strcmp(x, var_names[pos]) == 0){ // value already in the table
    var_vals[pos] = v;
  }
  else{
    if(known_var < MAX_VAR){
      int i;
      for(i=known_var; i>pos; i--){
	var_names[i] = var_names[i-1];
	var_vals[i] = var_vals[i-1];
	var_types[i] = var_types[i-1];
	var_mode[i] = var_mode[i -1 ]; 
      }
      var_names[pos] = strdup(x);
      var_vals[pos] = v;
      known_var++;
    }
    else {
      fprintf(stderr, "Too many variables\n");
      exit(1);
    }      
  }
}

void map_set_type(char * x, int v){
  int pos = get_position(x);
  if(var_names[pos] != NULL && strcmp(x, var_names[pos]) == 0){ // value already in the table
    var_types[pos] = v;
  }
  else{
    if(known_var < MAX_VAR){
      int i;
      for(i=known_var; i>pos; i--){
	var_names[i] = var_names[i-1];
	var_types[i] = var_types[i-1];
	var_vals[i] = var_vals[i-1];
	var_mode[i] = var_mode[i -1 ]; 
      }
      var_names[pos] = strdup(x);
      var_types[pos] = v;
      known_var++;
    }
    else {
      fprintf(stderr, "Too many variables\n");
      exit(1);
    }      
  }
}
