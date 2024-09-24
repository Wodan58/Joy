/*
    module  : manual.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef MANUAL_C
#define MANUAL_C

#include "manual.h"

/**
Q0  IGNORE_OK  2930  manual  :  ->
[IMPURE] Writes this manual of all Joy primitives to output file.
*/
void manual_(pEnv env)
{
    make_manual(0);
}
#endif
