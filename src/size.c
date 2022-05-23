/*
    module  : size.c
    version : 1.3
    date    : 05/17/22
*/
#ifndef SIZE_C
#define SIZE_C

/**
2090  size  :  A  ->  I
Integer I is the number of elements of aggregate A.
*/
PRIVATE void size_(pEnv env)
{
    long siz = 0;
    ONEPARAM("size");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(env->stck).set & ((long)1 << i))
                siz++;
        break;
    }
    case STRING_:
        siz = strlen(nodevalue(env->stck).str);
        break;
    case LIST_: {
        Index e = nodevalue(env->stck).lis;
        while (e) {
            e = nextnode1(e);
            siz++;
        }
        break;
    }
    default:
        BADAGGREGATE("size");
    }
    UNARY(INTEGER_NEWNODE, siz);
}
#endif
