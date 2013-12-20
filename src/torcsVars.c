#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "map.h"



#define RDONLYVARNUM 0 
#define RDWRVARNUM 1
#define VARNUM (RDONLYVARNUM + RDWRVARNUM)
#define LINELENGTH 100

struct TorcsVar
{
  char * _torcCName;
  idType _type;
  char * _LLVMLoad;
  int _LLVMRegNum;
};


struct TorcsVar rdOnlyVars [RDONLYVARNUM];
struct TorcsVar rdWrVars [RDWRVARNUM];




char * typeToLLVM (int CType)
{
  switch(CType){
  case TYPE_INT:
    return strdup("i32");
    break;
  case TYPE_VOID:
    return strdup("void");
    break;
  case TYPE_FLOAT:
    return strdup("float");
    break;
  default:
    fprintf(stderr, "Type : %d  \n", CType);
    perror ("Wrong type given somewhere.");
    exit(EXIT_FAILURE);
  }
}

void initVar(struct TorcsVar * var,char * name, idType type, char * load, int regNum )
{
  var->_torcCName = name;
  var->_type = type;
  var->_LLVMLoad = load;
  var->_LLVMRegNum = regNum;
  
}

void initData()
{
  /* Initializing Rd-Wr Variables
   */
  int i = 1 ;
  struct TorcsVar * var= rdWrVars;
  initVar (var, "$accel", TYPE_FLOAT, "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 1", i );
  i ++;
  var ++ ;


} 

int initTorcsVars()
{
  int reg = 1;
  int i1, i2;
  struct TorcsVar * var;

  initData();

  /* Initializing Read-Only Variables
   */
  for (i1 = 0; i1 < RDONLYVARNUM; i1++)
  {
    var = &rdOnlyVars[i1];
    map_set_mode(var->_torcCName, MODE_RDONLY);
    map_set_val (var->_torcCName, var->_LLVMRegNum );
    map_set_type(var->_torcCName, var->_type );
    reg ++;
  }
  /* Initializing Rd-Wr Variables
   */
  for (i2 = 0; i2 <RDWRVARNUM; i2++ )
  {
    var = &rdWrVars[i2];
    map_set_mode(var->_torcCName, MODE_RDWR);
    map_set_val (var->_torcCName, var->_LLVMRegNum );
    map_set_type(var->_torcCName, var->_type );
    reg ++;
  }

  return reg; 
}


char * getLLVMVarLoading()
{
  int i1, i2;
  struct TorcsVar * var;  
  char * buff = malloc (VARNUM * LINELENGTH * sizeof(char));
  if( buff == NULL)
  {
    perror("Error while creating LLVM pointers for TORCS variables");
    exit(EXIT_FAILURE);
  }
  else 
  {

    for (i1 =0; i1< RDONLYVARNUM; i1++)
    {
      var = &rdOnlyVars[i1];
      if (1) // Il faudra tester si le symbole a été utilisé
      {
	sprintf(buff, "\t%%_%d = %s\n", var->_LLVMRegNum, var->_LLVMLoad);
      }
    }	 
    for (i2 =0; i2< RDWRVARNUM; i2++)
    {
      var = &rdWrVars[i2];
      if(1)
      {
	sprintf(buff, "\t%%_%d = %s\n", var->_LLVMRegNum, var->_LLVMLoad);
      }
    }	 
  }
  return (buff);
}
char * getLLVMVarStoring()
{
  int i;
  struct TorcsVar * var;  
  char * buff = malloc (RDWRVARNUM * LINELENGTH * sizeof(char));
  if( buff == NULL)
  {
    perror("Error while creating LLVM pointers for TORCS variables");
    exit(EXIT_FAILURE);
  }
  else 
  {
    for (i =0; i< RDWRVARNUM; i++)
    {
      var = &rdWrVars[i];
      if (var-> _LLVMRegNum != map_get_val(var->_torcCName)) // Le symbole a été utilisé en écriture
      {
	char * type = typeToLLVM (var ->_type);
	sprintf(buff, "\tstore %s %%reg%d, %s* %%_%d\n", type, map_get_val(var->_torcCName), type, var->_LLVMRegNum);
	free(type);	  
      }
    }
  }
  return (buff);
}


//	store float 0.750000e+00, float* %accelCmd
