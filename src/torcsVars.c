#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "environment.h"
#include "util.h"
#include "symbol.h"

#include "torcsVars.h"

#define RDONLYVARNUM 9
#define RDWRVARNUM 5
#define INVVARNUM 9


struct TorcsVar
{
  char * _torcCName;
  char * _torcPointer;
  stype _type;
  char * _LLVMLoad;
  int _LLVMRegNum;
};

struct TorcsVar invisibleVars [INVVARNUM];
struct TorcsVar rdOnlyVars [RDONLYVARNUM];
struct TorcsVar rdWrVars [RDWRVARNUM];




void initVar(struct TorcsVar * var,char * name, char * ptrName, stype type, char * load, int regNum, int * i, struct TorcsVar ** varNum )
{
  var->_torcCName = name;
  var->_torcPointer= ptrName;
  var->_type = type;
  var->_LLVMLoad = load;
  var->_LLVMRegNum = regNum;
  (*i) ++;
  (*varNum) ++ ;
  
}

void initData()
{
  /* Initializing Rd-Wr Variables
   */
  int i = 1 ;
  struct TorcsVar * var= invisibleVars;


  /*Variables invisibles*/
  initVar (var,"" ,(char *) "ctrl", TYPE_FLOAT, (char *)  "getelementptr %struct.CarElt* %car, i32 0, i32 5", i, &i, &var );
  initVar (var,"" ,(char *) "public_car", TYPE_FLOAT, (char *) "getelementptr %struct.CarElt* %car, i32 0, i32 2", i, &i, &var );
  initVar (var,"" ,(char *) "private_car", TYPE_FLOAT, (char *) "getelementptr %struct.CarElt* %car, i32 0, i32 4", i, &i, &var );
  initVar (var,"" ,(char *) "pos_seg", TYPE_FLOAT, (char *) "getelementptr %struct.tPublicCar* %public_car, i32 0, i32 3", i, &i, &var );
  initVar (var,"" ,(char *) "dyngc", TYPE_FLOAT, (char *) "getelementptr %struct.tPublicCar* %public_car, i32 0, i32 0", i, &i, &var );
  initVar (var,"" ,(char *) "seg.addr", TYPE_FLOAT, (char *) "getelementptr %struct.tTrkLocPos* %pos_seg, i32 0, i32 0", i, &i, &var );
  initVar (var,"" ,(char *) "posd_pos", TYPE_FLOAT, (char *) "getelementptr %struct.tDynPt* %dyngc, i32 0, i32 0", i, &i, &var );
  initVar (var,"" ,(char *) "posd_vel", TYPE_FLOAT, (char *) "getelementptr %struct.tDynPt* %dyngc, i32 0, i32 1", i, &i, &var );
  initVar (var,"" ,(char *) "posd_acc", TYPE_FLOAT, (char *) "getelementptr %struct.tDynPt* %dyngc, i32 0, i32 2", i, &i, &var );

  var= rdWrVars;

  /*Variables en ecriture*/
  initVar (var, (char *) "$steer",(char *) "$steer_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 0", i, &i, &var );
  initVar (var, (char *) "$accel",(char *) "$accel_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 1", i, &i, &var );
  initVar (var, (char *) "$brake", (char *) "$brake_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 2", i, &i, &var );
  initVar (var, (char *) "$clutch", (char *) "$clutch_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 3", i, &i, &var );
  initVar (var, (char *) "$gear",(char *) "$gear_ptr", TYPE_INT, (char *) "getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 4", i, &i, &var );



  var = rdOnlyVars;

  /*Variables en lecture*/
  

  initVar (var,"$x" ,(char *) "$x_ptr", TYPE_FLOAT, (char *)  "getelementptr %struct.tPosd* %posd_pos, i32 0, i32 0", i, &i, &var );
  initVar (var,"$y" ,(char *) "$y_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_pos, i32 0, i32 1", i, &i, &var );
  initVar (var,"$z" ,(char *) "$z_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_pos, i32 0, i32 2", i, &i, &var );
  initVar (var,"$speedx" ,(char *) "$speedx_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_vel, i32 0, i32 0", i, &i, &var );
  initVar (var,"$speedy" ,(char *) "$speedy_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_vel, i32 0, i32 1", i, &i, &var );
  initVar (var,"$speedz" ,(char *) "$speedz_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_vel, i32 0, i32 2", i, &i, &var );
  initVar (var,"$accelx" ,(char *) "$accelx_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_acc, i32 0, i32 0", i, &i, &var );
  initVar (var,"$accely" ,(char *) "$accely_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_acc, i32 0, i32 1", i, &i, &var );
  initVar (var,"$accelz" ,(char *) "$accelz_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPosd* %posd_acc, i32 0, i32 2", i, &i, &var );
  /* Abandon de la variable rpm (Le compilateur LLVM renvoie une erreur d'incompatibilite de type)*/
  //initVar (var,"$rpm" ,(char *) "$rpm_ptr", TYPE_FLOAT, (char *) "getelementptr %struct.tPrivCar* %private_car, i32 8", i, &i, &var );



} 

int initTorcsVars(struct environment * env)
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
    struct symbol_s * s = newSymbol(var->_torcCName, var->_LLVMRegNum, var->_type, IS_TORCSIAN);
    setSymbol(env, s,var->_torcCName );
    reg ++;
  }
  /* Initializing Rd-Wr Variables
   */
  for (i2 = 0; i2 <RDWRVARNUM; i2++ )
  {
    var = &rdWrVars[i2];
    struct symbol_s * s = newSymbol(var->_torcCName, var->_LLVMRegNum, var->_type, IS_TORCSIAN|IS_WR);
    setSymbol(env, s, var->_torcCName );
    reg ++;
  }

  return 200; 
}



void getLLVMVarLoading(struct environment * env, char bigbuffer[] )
{
  int i0, i1, i2;
  struct TorcsVar * var;
  for (i0 =0; i0< INVVARNUM; i0++)
  {
   var = &invisibleVars[i0];
   sprintf(&bigbuffer[strlen(bigbuffer)], "\t%%%s = %s\n", var->_torcPointer, var->_LLVMLoad);
 }   
 for (i1 =0; i1< RDONLYVARNUM; i1++)
 {
   var = &rdOnlyVars[i1];

   sprintf(&bigbuffer[strlen(bigbuffer)], "\t%%%s = %s\n\t%%r%d = load %s* %%%s\n",
     var->_torcPointer, var->_LLVMLoad, var->_LLVMRegNum, typeToLLVM(var->_type), var->_torcPointer);

 }   
 for (i2 =0; i2< RDWRVARNUM; i2++)
 {
  var = &rdWrVars[i2];
  sprintf(&bigbuffer[strlen(bigbuffer)], "\t%%%s = %s\n\t%%r%d = load %s* %%%s\n",
     var->_torcPointer, var->_LLVMLoad, var->_LLVMRegNum, typeToLLVM(var->_type), var->_torcPointer);
}	 
}
void getLLVMVarStoring(struct environment * env, char bigbuffer[])
{
  int i;
  struct TorcsVar * var;  

  for (i =0; i< RDWRVARNUM; i++)
  {
    var = &rdWrVars[i];

    char * type = typeToLLVM (var ->_type);
    sprintf(&bigbuffer[strlen(bigbuffer)], "\tstore %s %%r%d, %s* %%%s\n", type, getSymbolFromEnv(env, var->_torcCName)->regNum , type, var->_torcPointer);
  }

}



void declareFunctionMasks(struct environment * toplevel)
{
/*  struct symbol_s *s;
  s = newSymbol("get_track_angle", 0, TYPE_FLOAT, IS_FUNC | IS_TORCSIAN);
  s->params = NULL;
  setSymbol(toplevel, s, "road_angle");

  s = newSymbol("get_car_yaw", 0, TYPE_FLOAT, IS_FUNC | IS_TORCSIAN);
  s->params = NULL;
  setSymbol(toplevel, s, "car_angle");*/

}

//	store float 0.750000e+00, float* %accelCmd
