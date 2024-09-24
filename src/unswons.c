/*
    module  : unswons.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef UNSWONS_C
#define UNSWONS_C

/**
Q0  OK  2120  unswons  :  A  ->  R F
R and F are the rest and the first of non-empty aggregate A.
*/
void unswons_(pEnv env)
{
    int i = 0;
    char *str;
    uint64_t set;

    ONEPARAM("unswons");
    switch (nodetype(env->stck)) {
    case SET_:
	set = nodevalue(env->stck).set;
	CHECKEMPTYSET(set, "unswons");
	while (!(set & ((int64_t)1 << i)))
	    i++;
	UNARY(SET_NEWNODE, set & ~((int64_t)1 << i));
	NULLARY(INTEGER_NEWNODE, i);
	break;
    case STRING_:
	str = strdup((char *)&nodevalue(env->stck));
	CHECKEMPTYSTRING(str, "unswons");
	UNARY(STRING_NEWNODE, str + 1);
	NULLARY(CHAR_NEWNODE, *str);
	free(str);
	break;
    case LIST_:
	SAVESTACK;
	CHECKEMPTYLIST(nodevalue(SAVED1).lis, "unswons");
	UNARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
	GNULLARY(nodevalue(SAVED1).lis);
	POP(env->dump);
	break;
    default:
	BADAGGREGATE("unswons");
    }
}
#endif
