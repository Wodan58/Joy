/*
    module  : fget.c
    version : 1.7
    date    : 09/07/23
*/
#ifndef FGET_C
#define FGET_C

/**
OK 3180  fget  :  S  ->  S F
[EXT] Reads a factor from stream S and pushes it onto stack.
*/
PRIVATE void fget_(pEnv env)
{
    FILE *fp;

    ONEPARAM("fget");
    FILE("fget");
    fp = nodevalue(env->stck).fil;
    if (!redirect(env, "fget", fp))	/* conditional switch of file */
	return;
    getsym(env);
    switch (env->symb) {
    case ATOM:
    case CHAR_:
    case INTEGER_:
    case STRING_:
    case FLOAT_:
    case LBRACE:
    case LBRACK:
	readfactor(env);
	break;
    default:
	env->bucket.num = env->symb;
	env->stck = newnode(env, UNKNOWN_, env->bucket, env->stck);
	break;
    }
}
#endif
