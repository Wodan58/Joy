/*
    module  : fseek.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef FSEEK_C
#define FSEEK_C

/**
Q0  OK  1980  fseek  :  S P W  ->  S B
[FOREIGN] Stream S is repositioned to position P relative to whence-point W,
where W = 0, 1, 2 for beginning, current position, end respectively.
*/
void fseek_(pEnv env)
{
    int whence;
    int64_t pos;

    THREEPARAMS("fseek");
    INTEGERS2("fseek");
    whence = nodevalue(env->stck).num;
    POP(env->stck);
    pos = nodevalue(env->stck).num;
    POP(env->stck);
    ISFILE("fseek");
    NULLARY(BOOLEAN_NEWNODE, !!fseek(nodevalue(env->stck).fil, pos, whence));
}
#endif
