/*
    module  : fgets.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef FGETS_C
#define FGETS_C

/**
OK 1880  fgets  :  S  ->  S L
[FOREIGN] L is the next available line (as a string) from stream S.
*/
void fgets_(pEnv env)
{
    char *buf;
    size_t leng, size = INPLINEMAX;

    ONEPARAM("fgets");
    FILE("fgets");
#ifdef NOBDW
    buf = malloc(size);
#else
    buf = GC_malloc_atomic(size);
#endif
    buf[leng = 0] = 0;
    while (fgets(buf + leng, size - leng, nodevalue(env->stck).fil)) {
	if ((leng = strlen(buf)) > 0 && buf[leng - 1] == '\n')
	    break;
#ifdef NOBDW
	buf = realloc(buf, size <<= 1);
#else
	buf = GC_realloc(buf, size <<= 1);
#endif
    }
    NULLARY(STRING_NEWNODE, buf);
#ifdef NOBDW
    free(buf);
#endif
}
#endif
