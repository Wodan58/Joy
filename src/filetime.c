/*
    module  : filetime.c
    version : 1.2
    date    : 09/04/23
*/
#ifndef FILETIME_C
#define FILETIME_C

#include <sys/stat.h>

/**
OK 3190  filetime  :  F  ->  T
[EXT] T is the modification time of file F.
*/
void filetime_(pEnv env)
{
    int rv;
    struct stat buf;

    ONEPARAM("filetime");
    STRING("filetime");
    rv = stat(nodevalue(env->stck).str, &buf);
    UNARY(INTEGER_NEWNODE, rv < 0 ? 0 : buf.st_mtime);
}
#endif
