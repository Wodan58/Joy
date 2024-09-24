/*
    module  : __html_manual.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef __HTML_MANUAL_C
#define __HTML_MANUAL_C

#include "manual.h"

/**
Q0  IGNORE_OK  2940  __html_manual  :  ->
[IMPURE] Writes this manual of all Joy primitives to output file in HTML style.
*/
void __html_manual_(pEnv env)
{
    make_manual(1);
}
#endif
