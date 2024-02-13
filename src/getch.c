/*
    module  : getch.c
    version : 1.7
    date    : 02/05/24
*/
#ifndef GETCH_C
#define GETCH_C

/**
OK 3160  getch  :  ->  N
[IMPURE] Reads a character from input and puts it onto stack.
*/
PRIVATE void getch_(pEnv env)
{
    if (env->ignore) {
	env->bucket.num = 0;
	env->stck = newnode(env, CHAR_, env->bucket, env->stck);
	return;
    }
    NULLARY(CHAR_NEWNODE, fgetc(env->srcfile));
}
#endif
