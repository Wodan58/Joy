/*
    module  : filetime.c
    version : 1.13
    date    : 02/04/26
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
	buf = GC_malloc_atomic(sizeof(struct stat));
	if (fstat(fileno(fp), buf) >= 0)
	    mtime = buf->st_mtime;
	fclose(fp);
    }
    UNARY(INTEGER_NEWNODE, mtime);
}
#endif
