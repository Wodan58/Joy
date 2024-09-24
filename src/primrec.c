/*
    module  : primrec.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef PRIMREC_C
#define PRIMREC_C

/**
Q2  OK  2820  primrec  :  X [I] [C]  ->  R
Executes I to obtain an initial value R0.
For integer X uses increasing positive integers to X, combines by C for new R.
For aggregate X uses successive members and combines by C for new R.
*/
void primrec_(pEnv env)
{
    char *str;
    uint64_t set;
    int i = 0, n = 0;

    THREEPARAMS("primrec");
    TWOQUOTES("primrec");
    SAVESTACK;
    env->stck = nextnode3(env->stck);
    switch (nodetype(SAVED3)) {
    case LIST_:
	env->dump1 = LIST_NEWNODE(nodevalue(SAVED3).lis, env->dump1);
	for (; DMP1; DMP1 = nextnode1(DMP1)) {
	    GNULLARY(DMP1);
	    n++;
	}
	POP(env->dump1);
	break;
    case STRING_:
	for (str = strdup((char *)&nodevalue(SAVED3)); str[i]; i++) {
	    NULLARY(CHAR_NEWNODE, str[i]);
	    n++;
	}
	free(str);
	break;
    case SET_:
	set = nodevalue(SAVED3).set;
	for (; i < SETSIZE; i++)
	    if (set & ((int64_t)1 << i)) {
		NULLARY(INTEGER_NEWNODE, i);
		n++;
	    }
	break;
    case INTEGER_:
	for (i = nodevalue(SAVED3).num; i > 0; i--) {
	    NULLARY(INTEGER_NEWNODE, i);
	    n++;
	}
	break;
    default:
	BADDATA("primrec");
    }
    exeterm(env, nodevalue(SAVED2).lis);
    for (i = 1; i <= n; i++)
	exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
