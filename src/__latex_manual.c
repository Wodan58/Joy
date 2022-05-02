/*
    module  : __latex_manual.c
    version : 1.2
    date    : 05/02/22
*/
#ifndef __LATEX_MANUAL_C
#define __LATEX_MANUAL_C

#include "manual.h"

/**
2970  __latex_manual  :  ->
Writes this manual of all Joy primitives to output file in Latex style
but without the head and tail.
*/
PRIVATE void __latex_manual_(pEnv env) { make_manual(2); }



#endif
