/*
    module  : fseek.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FSEEK_C
#define FSEEK_C

/**
1990  fseek  :  S P W  ->  S B
Stream S is repositioned to position P relative to whence-point W,
where W = 0, 1, 2 for beginning, current position, end respectively.
*/
PRIVATE void fseek_(pEnv env)
{
    long pos;
    int whence;

    THREEPARAMS("fseek");
    INTEGER("fseek");
    INTEGER2("fseek");
    whence = nodevalue(env->stck).num;
    POP(env->stck);
    pos = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fseek");
    NULLARY(BOOLEAN_NEWNODE, !!fseek(nodevalue(env->stck).fil, pos, whence));
}
#endif
