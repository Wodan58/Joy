/*
    module  : manual.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef MANUAL_C
#define MANUAL_C

#include "manual.h"

/**
OK 2930  manual  :  ->
[IMPURE] Writes this manual of all Joy primitives to output file.
*/
PRIVATE void manual_(pEnv env)
{
    if (env->ignore)
	return;
    make_manual(0);
}
#endif
