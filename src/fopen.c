/*
    module  : fopen.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef FOPEN_C
#define FOPEN_C

/**
OK 1890  fopen  :  P M  ->  S
[FOREIGN] The file system object with pathname P is opened with mode M
(r, w, a, etc.) and stream object S is pushed; if the open fails, file:NULL
is pushed.
*/
void fopen_(pEnv env)
{
    TWOPARAMS("fopen");
    STRING("fopen");
    STRING2("fopen");
#ifdef NOBDW
    BINARY(FILE_NEWNODE, fopen((char *)&nodevalue(nextnode1(env->stck)),
			    (char *)&nodevalue(env->stck)));
#else
    BINARY(FILE_NEWNODE, fopen(nodevalue(nextnode1(env->stck)).str,
			    nodevalue(env->stck).str));
#endif
}
#endif
