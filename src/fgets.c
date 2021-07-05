/*
    module  : fgets.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FGETS_C
#define FGETS_C

/**
1890  fgets  :  S  ->  S L
L is the next available line (as a string) from stream S.
*/
PRIVATE void fgets_(pEnv env)
{
    char *buf;
    size_t leng, size = INPLINEMAX;

    ONEPARAM("fgets");
    FILE("fgets");
    buf = GC_malloc_atomic(size);
    buf[leng = 0] = 0;
    while (fgets(buf + leng, size - leng, nodevalue(env->stck).fil)) {
        if ((leng = strlen(buf)) > 0 && buf[leng - 1] == '\n')
            break;
        buf = GC_realloc(buf, size <<= 1);
    }
    NULLARY(STRING_NEWNODE, buf);
}
#endif
