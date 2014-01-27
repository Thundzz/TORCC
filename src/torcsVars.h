#ifndef TORCSVARS_H
#define TORCSVARS_H

#include "environment.h"

/* Function prototypes */

int initTorcsVars(struct environment * env);


void getLLVMVarLoading(struct environment * env, char bigbuffer[] );
void getLLVMVarStoring(struct environment * env, char bigbuffer[]);
void declareFunctionMasks(struct environment * toplevel);

#endif
