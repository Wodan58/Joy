/*
    module  : size.c
    version : 1.7
    date    : 03/05/24
*/
#ifndef SIZE_C
#define SIZE_C

/**
OK 2080  size  :  A  ->  I
Integer I is the number of elements of aggregate A.
*/
PRIVATE void size_(pEnv env)
{
    int i;
    Index list;
    int64_t size = 0;

    ONEPARAM("size");
    switch (nodetype(env->stck)) {
    case SET_:
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(env->stck).set & ((int64_t)1 << i))
                size++;
        break;
    case STRING_:
        size = strlen(nodevalue(env->stck).str);
        break;
    case LIST_:
        list = nodevalue(env->stck).lis;
        while (list) {
            list = nextnode1(list);
            size++;
        }
        break;
    default:
        BADAGGREGATE("size");
    }
    UNARY(INTEGER_NEWNODE, size);
}
#endif
