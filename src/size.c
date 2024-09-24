/*
    module  : size.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef SIZE_C
#define SIZE_C

/**
Q0  OK  2080  size  :  A  ->  I
Integer I is the number of elements of aggregate A.
*/
void size_(pEnv env)
{
    Index list;
    int i, size = 0;

    ONEPARAM("size");
    switch (nodetype(env->stck)) {
    case SET_:
	for (i = 0; i < SETSIZE; i++)
	    if (nodevalue(env->stck).set & ((int64_t)1 << i))
		size++;
	break;
    case STRING_:
#ifdef NOBDW
	size = nodeleng(env->stck);
#else
	size = strlen(nodevalue(env->stck).str);
#endif
	break;
    case LIST_:
	for (list = nodevalue(env->stck).lis; list; list = nextnode1(list))
	    size++;
	break;
    default:
	BADAGGREGATE("size");
    }
    UNARY(INTEGER_NEWNODE, size);
}
#endif
