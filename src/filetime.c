/*
    module  : filetime.c
    version : 1.8
    date    : 08/12/24
*/
#ifndef FILETIME_C
#define FILETIME_C

#include <sys/stat.h>

/**
OK 3160  filetime  :  F  ->  T
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
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    mtime = 0;
    if ((fp = fopen(str, "r")) != 0) {
	buf = check_malloc(sizeof(struct stat));
	if (fstat(fileno(fp), buf) >= 0)
	    mtime = buf->st_mtime;
	free(buf);
	fclose(fp);
    }
    UNARY(INTEGER_NEWNODE, mtime);
}
#endif
