#include "environment.h"
#include "hashtable.h"
#include "symbol.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>



/* Ne pas mettre une puissance de 2 ici */

#define DEFAULTTABLESIZE 1337

struct symbol_s * getSymbolFromEnv(struct environment * env,  char * key)
{
	if (env == NULL)
		return NULL;
	else
	{
		struct symbol_s * s = ht_get (env->table, key);
		if (s==NULL)
		{
			return getSymbolFromEnv(env->englobant, key);
		}
		else
		{
			return s;
		}

	}

}


struct environment * createEnvironment ()
{
	struct environment * env = (struct environment *) malloc(sizeof(struct environment));
	env->table = ht_create(DEFAULTTABLESIZE);
	env->englobant = NULL;

	return env;
}

void setSymbol(struct environment * env, struct symbol_s * sym, char * key)
{
	ht_set(env->table, key, sym);
}



struct environment * englober(struct environment * newenv, struct environment * used)
{
	if(NULL == newenv)
	{
		newenv = createEnvironment();
	}
	newenv->englobant = used;
	return newenv;
}

struct environment * deglober(struct environment * env)
{
	struct environment * tmp = env;
	env = env-> englobant;
	freeEnvironment(tmp, PLEASEFREESYMBOLS);
	return env;
}

void freeEnvironment(struct environment * env, int freeSymbols)
{
	if(env !=NULL)
	{
		ht_free(env->table, freeSymbols);
		free(env);
	}
}