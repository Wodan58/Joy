/*
    module  : boolean.h
    version : 1.5
    date    : 11/15/24
*/
#ifndef BOOLEAN_H
#define BOOLEAN_H

/*
 * A truth value is expected in a condition. That truth value need not be a
 * boolean, but if it is anything else, it should be converted into one.
 * That is the task of get_boolean. This file should be included whenever
 * a condition is needed.
 */
static int get_boolean(pEnv env, Index node)
{
    int rv = 0;

    switch (nodetype(node)) {
    /*
     * USR_ and ANON_FUNCT_ cannot be 0, so they always evaluate to true.
     */
    case USR_:
    case ANON_FUNCT_:
	rv = 1;	/* assume true */
	break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
	return nodevalue(node).num != 0;
    case SET_:
	return nodevalue(node).set != 0;
    case STRING_:
    case BIGNUM_:
#ifdef NOBDW
	return nodeleng(node) != 0;
#else
	return *nodevalue(node).str != 0;
#endif
    case LIST_:
	return nodevalue(node).lis != 0;
    case FLOAT_:
	return nodevalue(node).dbl != 0;
    case FILE_:
	return nodevalue(node).fil != 0;
    }
    return rv;
}
#endif
