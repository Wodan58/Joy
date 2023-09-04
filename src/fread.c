/*
    module  : fread.c
    version : 1.5
    date    : 09/04/23
*/
#ifndef FREAD_C
#define FREAD_C

/**
OK 1900  fread  :  S I  ->  S L
I bytes are read from the current position of stream S
and returned as a list of I integers.
*/
PRIVATE void fread_(pEnv env)
{
    unsigned char *buf;
    int64_t count;

    TWOPARAMS("fread");
    INTEGER("fread");
    count = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fread");
    buf = GC_malloc_atomic(count);
    env->dump1 = LIST_NEWNODE(0, env->dump1);
    for (count = fread(buf, 1, count, nodevalue(env->stck).fil) - 1;
         count >= 0; count--)
        DMP1 = INTEGER_NEWNODE((int64_t)buf[count], DMP1);
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
}
#endif
