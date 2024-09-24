/*
    module  : small.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef SMALL_C
#define SMALL_C

/**
Q0  OK  2210  small  :  X  ->  B
Tests whether aggregate X has 0 or 1 members, or numeric 0 or 1.
*/
void small_(pEnv env)
{
    int i = 0, small = 0;

    ONEPARAM("small");
    switch (nodetype(env->stck)) {
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
	small = nodevalue(env->stck).num < 2;
	break;
    case SET_:
	if (nodevalue(env->stck).set == 0)
	    small = 1;
	else {
	    while (!(nodevalue(env->stck).set & ((int64_t)1 << i)))
		i++;
	    small = (nodevalue(env->stck).set & ~((int64_t)1 << i)) == 0;
	}
	break;
    case STRING_:
#ifdef NOBDW
	small = nodeleng(env->stck) < 2;
#else
	small = !*nodevalue(env->stck).str || !nodevalue(env->stck).str[1];
#endif
	break;
    case LIST_:
	small = !nodevalue(env->stck).lis ||
		!nextnode1(nodevalue(env->stck).lis);
	break;
    default:
	BADDATA("small");
    }
    UNARY(BOOLEAN_NEWNODE, small);
}
#endif
