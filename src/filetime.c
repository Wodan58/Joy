/*
    module  : filetime.c
    version : 1.6
    date    : 06/21/24
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
    int rv;
    struct stat buf;

    ONEPARAM("filetime");
    STRING("filetime");
#ifdef NOBDW
    rv = stat((char *)&nodevalue(env->stck), &buf);
#else
    rv = stat(nodevalue(env->stck).str, &buf);
#endif
    UNARY(INTEGER_NEWNODE, rv < 0 ? 0 : buf.st_mtime);
}
#endif
