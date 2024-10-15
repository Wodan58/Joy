/*
    module  : genrec.c
    version : 1.11
    date    : 10/11/24
*/
#ifndef GENREC_C
#define GENREC_C

#include "cons.c"
#include "genrecaux.c"

/**
Q4  OK  2740  genrec  :  [B] [T] [R1] [R2]  ->  ...
Executes B, if that yields true, executes T.
Else executes R1 and then [[[B] [T] [R1] R2] genrec] R2.
*/
void genrec_(pEnv env)
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_(env);
    cons_(env);
    cons_(env);
    genrecaux_(env);
}
#endif
