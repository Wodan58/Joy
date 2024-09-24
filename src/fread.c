/*
    module  : fread.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef FREAD_C
#define FREAD_C

/**
Q0  OK  1900  fread  :  S I  ->  S L
[FOREIGN] I bytes are read from the current position of stream S
and returned as a list of I integers.
*/
void fread_(pEnv env)
{
    int count;
    unsigned char *buf;

    TWOPARAMS("fread");
    INTEGER("fread");
    count = nodevalue(env->stck).num;
    POP(env->stck);
    ISFILE("fread");
    buf = malloc(count);
    env->dump1 = LIST_NEWNODE(0, env->dump1);
    for (count = fread(buf, 1, count, nodevalue(env->stck).fil) - 1;
	 count >= 0; count--)
	DMP1 = INTEGER_NEWNODE(buf[count], DMP1);
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
    free(buf);
}
#endif
