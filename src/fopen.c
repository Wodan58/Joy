/*
    module  : fopen.c
    version : 1.9
    date    : 10/18/24
*/
#ifndef FOPEN_C
#define FOPEN_C

/**
Q0  OK  1890  fopen  :  P M  ->  S
[FOREIGN] The file system object with pathname P is opened with mode M
(r, w, a, etc.) and stream object S is pushed; if the open fails, file:NULL
is pushed.
*/
void fopen_(pEnv env)
{
    char *path, *mode;

    TWOPARAMS("fopen");
    STRING("fopen");
    STRING2("fopen");
    path = GETSTRING(nextnode1(env->stck));
    mode = GETSTRING(env->stck);
    BINARY(FILE_NEWNODE, fopen(path, mode));
}
#endif
