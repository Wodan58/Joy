/*
    module  : compare.h
    version : 1.22
    date    : 10/18/24
*/
#ifndef COMPARE_H
#define COMPARE_H

static int is_null(pEnv env, Index node)
{
    switch (nodetype(node)) {
    case USR_:
    case ANON_FUNCT_:
	break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
	return !nodevalue(node).num;
    case SET_:
	return !nodevalue(node).set;
    case STRING_:
    case BIGNUM_:
#ifdef NOBDW
	return !nodeleng(node);
#else
	return !*nodevalue(node).str;
#endif
    case LIST_:
	return !nodevalue(node).lis;
    case FLOAT_:
	return !nodevalue(node).dbl;
    case FILE_:
	return !nodevalue(node).fil;
    }
    return 0;
}

int Compare(pEnv env, Index first, Index second)
{
    FILE *fp1, *fp2;
    int type1, type2;
    double dbl1 = 0, dbl2 = 0;
    int64_t num1 = 0, num2 = 0;
    char *name1 = "", *name2 = "";

    if (is_null(env, first) && is_null(env, second))	/* only one nothing */
	return 0;
    type1 = nodetype(first);
    type2 = nodetype(second);
    switch (type1) {
    case USR_:
	name1 = vec_at(env->symtab, nodevalue(first).ent).name;
	switch (type2) {
	case USR_:
	    name2 = vec_at(env->symtab, nodevalue(second).ent).name;
	    goto cmpstr;
	case ANON_FUNCT_:
	    name2 = nickname(operindex(env, nodevalue(second).proc));
	    goto cmpstr;
	case STRING_:
	case BIGNUM_:
	    name2 = GETSTRING(second);
	    goto cmpstr;
	}
	break;
    case ANON_FUNCT_:
	name1 = nickname(operindex(env, nodevalue(first).proc));
	switch (type2) {
	case USR_:
	    name2 = vec_at(env->symtab, nodevalue(second).ent).name;
	    goto cmpstr;
	case ANON_FUNCT_:
	    name2 = nickname(operindex(env, nodevalue(second).proc));
	    goto cmpstr;
	case STRING_:
	case BIGNUM_:
	    name2 = GETSTRING(second);
	    goto cmpstr;
	}
	break;
    case BOOLEAN_:
	num1 = nodevalue(first).num;
	switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	    num2 = nodevalue(second).num;
	    goto cmpnum;
	case FLOAT_:
	    dbl1 = num1;
	    dbl2 = nodevalue(second).dbl;
	    goto cmpdbl;
	}
	break;
    case CHAR_:
	num1 = nodevalue(first).num;
	switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	    num2 = nodevalue(second).num;
	    goto cmpnum;
	case FLOAT_:
	    dbl1 = num1;
	    dbl2 = nodevalue(second).dbl;
	    goto cmpdbl;
	}
	break;
    case INTEGER_:
	num1 = nodevalue(first).num;
	switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	    num2 = nodevalue(second).num;
	    goto cmpnum;
	case FLOAT_:
	    dbl1 = num1;
	    dbl2 = nodevalue(second).dbl;
	    goto cmpdbl;
	}
	break;
    case SET_:
	num1 = nodevalue(first).set;
	switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	case FLOAT_:
	    num2 = nodevalue(second).num;
	    goto cmpnum;
	}
	break;
    case LIST_:
	break;
    case STRING_:
    case BIGNUM_:
	name1 = GETSTRING(first);
	switch (type2) {
	case USR_:
	    name2 = vec_at(env->symtab, nodevalue(second).ent).name;
	    goto cmpstr;
	case ANON_FUNCT_:
	    name2 = nickname(operindex(env, nodevalue(second).proc));
	    goto cmpstr;
	case STRING_:
	case BIGNUM_:
	    name2 = GETSTRING(second);
	    goto cmpstr;
	}
	break;
    case FLOAT_:
	dbl1 = nodevalue(first).dbl;
	switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	    dbl2 = nodevalue(second).num;
	    goto cmpdbl;
	case SET_:
	    num1 = nodevalue(first).num;
	    num2 = nodevalue(second).num;
	    goto cmpnum;
	case FLOAT_:
	    dbl2 = nodevalue(second).dbl;
	    goto cmpdbl;
	}
	break;
    case FILE_:
	fp1 = nodevalue(first).fil;
	switch (type2) {
	case FILE_:
	    fp2 = nodevalue(second).fil;
	    return fp1 < fp2 ? -1 : fp1 > fp2;
	}
	break;
    }
    return 1;	/* unequal */
cmpnum:
    return num1 < num2 ? -1 : num1 > num2;
cmpdbl:
    return dbl1 < dbl2 ? -1 : dbl1 > dbl2;
cmpstr:
    num1 = strcmp(name1, name2);
    return num1 < 0 ? -1 : num1 > 0;
}
#endif
