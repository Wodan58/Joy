/*
    module  : fgets.c
    version : 1.10
    date    : 10/28/24
*/
#ifndef FGETS_C
#define FGETS_C

/**
Q0  OK  1880  fgets  :  S  ->  S L
[FOREIGN] L is the next available line (as a string) from stream S.
*/
void fgets_(pEnv env)
{
    char *buf;
#ifdef NOBDW
    char *tmp;
#endif
    size_t leng, size = INPLINEMAX;

    ONEPARAM("fgets");
    ISFILE("fgets");
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
	if ((tmp = realloc(buf, size <<= 1)) == 0)
	    break;	/* LCOV_EXCLUDE_LINE */
	buf = tmp;
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
