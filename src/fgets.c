/*
    module  : fgets.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef FGETS_C
#define FGETS_C

/**
OK 1880  fgets  :  S  ->  S L
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
