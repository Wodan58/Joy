/*
    module  : __html_manual.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef __HTML_MANUAL_C
#define __HTML_MANUAL_C

#include "manual.h"

/**
OK 2940  __html_manual  :  ->
[IMPURE] Writes this manual of all Joy primitives to output file in HTML style.
*/
PRIVATE void __html_manual_(pEnv env)
{
    if (env->ignore)
	return;
    make_manual(1);
}
#endif
