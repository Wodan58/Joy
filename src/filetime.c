/*
    module  : filetime.c
    version : 1.11
    date    : 10/28/24
*/
#ifndef FILETIME_C
#define FILETIME_C

#include <sys/stat.h>

/**
Q0  OK  3160  filetime  :  F  ->  T
[FOREIGN] T is the modification time of file F.
*/
void filetime_(pEnv env)
{
    FILE *fp;
    char *str;
    time_t mtime;	/* modification time */
    struct stat *buf;	/* struct stat is big */

    ONEPARAM("filetime");
    STRING("filetime");
    str = GETSTRING(env->stck);
    mtime = 0;
    if ((fp = fopen(str, "r")) != 0) {
#ifdef NOBDW
	buf = malloc(sizeof(struct stat));
#else
	buf = GC_malloc_atomic(sizeof(struct stat));
#endif
	if (fstat(fileno(fp), buf) >= 0)
	    mtime = buf->st_mtime;
#ifdef NOBDW
	free(buf);
#endif
	fclose(fp);
    }
    UNARY(INTEGER_NEWNODE, mtime);
}
#endif
