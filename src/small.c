/*
    module  : small.c
    version : 1.6
    date    : 03/05/24
*/
#ifndef SMALL_C
#define SMALL_C

/**
OK 2210  small  :  X  ->  B
Tests whether aggregate X has 0 or 1 members, or numeric 0 or 1.
*/
PRIVATE void small_(pEnv env)
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
        small = nodevalue(env->stck).str[0] == '\0'
		|| nodevalue(env->stck).str[1] == '\0';
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
