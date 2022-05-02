/*
    module  : fwrite.c
    version : 1.3
    date    : 05/02/22
*/
#ifndef FWRITE_C
#define FWRITE_C

/**
1920  fwrite  :  S L  ->  S
A list of integers are written as bytes to the current position of stream S.
*/
PRIVATE void fwrite_(pEnv env)
{
    int length, i;
    unsigned char *buff;
    Index n;

    TWOPARAMS("fwrite");
    LIST("fwrite");
    for (n = nodevalue(env->stck).lis, length = 0; n;
         n = nextnode1(n), length++)
        CHECKNUMERIC(n, "fwrite");
    buff = GC_malloc_atomic(length);
    for (n = nodevalue(env->stck).lis, i = 0; n; n = nextnode1(n), i++)
        buff[i] = (char)nodevalue(n).num;
    POP(env->stck);
    FILE("fwrite");
    fwrite(buff, (size_t)length, (size_t)1, nodevalue(env->stck).fil);
}
#endif
