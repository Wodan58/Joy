/*
    module  : genrec.c
    version : 1.8
    date    : 08/26/23
*/
#ifndef GENREC_C
#define GENREC_C

/**
OK 2760  genrec  :  [B] [T] [R1] [R2]  ->  ...
Executes B, if that yields true, executes T.
Else executes R1 and then [[[B] [T] [R1] R2] genrec] R2.
*/
PRIVATE void genrec_(pEnv env)
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_(env);
    cons_(env);
    cons_(env);
    _genrec_(env);
}
#endif
