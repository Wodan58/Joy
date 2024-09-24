/*
    module  : fwrite.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef FWRITE_C
#define FWRITE_C

/**
Q0  OK  1910  fwrite  :  S L  ->  S
[FOREIGN] A list of integers are written as bytes to the current position of
stream S.
*/
void fwrite_(pEnv env)
{
    int i;
    Index n;
    unsigned char *buf;

    TWOPARAMS("fwrite");
    LIST("fwrite");
    /* check that each member of the list is numeric; also count the length */
    for (n = nodevalue(env->stck).lis, i = 0; n; n = nextnode1(n), i++)
	CHECKNUMERIC(n, "fwrite");
#ifdef NOBDW
    buf = malloc(i);
#else
    buf = GC_malloc_atomic(i);
#endif
    /* copy the list of integers to the character array; truncating integers */
    for (n = nodevalue(env->stck).lis, i = 0; n; n = nextnode1(n), i++)
	buf[i] = (unsigned char)nodevalue(n).num;
    POP(env->stck);
    ISFILE("fwrite");
    fwrite(buf, i, 1, nodevalue(env->stck).fil);
#ifdef NOBDW
    free(buf);
#endif
}
#endif
