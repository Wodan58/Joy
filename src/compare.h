/*
    module  : compare.h
    version : 1.10
    date    : 05/23/23
*/
#ifndef COMPARE_H
#define COMPARE_H

PUBLIC int Compare(pEnv env, Index first, Index second)
{
    FILE *fp1, *fp2;
    int type1, type2;
    char *name1, *name2;
    double dbl, dbl1, dbl2;
    int64_t num, num1, num2;

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
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case LIST_:
        case FLOAT_:
        case FILE_:
            return 1; /* unequal */
        case STRING_:
            name2 = nodevalue(second).str;
            goto cmpstr;
        default:
            name2 = nickname(type2);
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
        default:
            return 1; /* unequal */
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
        default:
            return 1; /* unequal */
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
        default:
            return 1;
        }
        break;
    case SET_:
        num1 = nodevalue(first).num;
        switch (type2) {
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
        case SET_:
            num2 = nodevalue(second).set;
            goto cmpnum;
        default:
            return 1; /* unequal */
        }
        break;
    case STRING_:
        name1 = nodevalue(first).str;
        switch (type2) {
        case USR_:
            name2 = vec_at(env->symtab, nodevalue(second).ent).name;
            goto cmpstr;
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case LIST_:
        case FLOAT_:
        case FILE_:
            return 1; /* unequal */
        case STRING_:
            name2 = nodevalue(second).str;
            goto cmpstr;
        default:
            name2 = nickname(type2);
            goto cmpstr;
        }
        break;
    case LIST_:
        return 1; /* unequal */
        break;
    case FLOAT_:
        dbl1 = nodevalue(first).dbl;
        switch (type2) {
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            dbl2 = nodevalue(second).num;
            goto cmpdbl;
        case FLOAT_:
            dbl2 = nodevalue(second).dbl;
            goto cmpdbl;
        default:
            return 1; /* unequal */
        }
        break;
    case FILE_:
        fp1 = nodevalue(first).fil;
        switch (type2) {
        case FILE_:
            fp2 = nodevalue(second).fil;
            return fp1 - fp2 < 0 ? -1 : fp1 - fp2 > 0;
        default:
            return 1; /* unequal */
        }
        break;
    default:
        name1 = nickname(type1);
        switch (type2) {
        case USR_:
            name2 = vec_at(env->symtab, nodevalue(second).ent).name;
            goto cmpstr;
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case LIST_:
        case FLOAT_:
        case FILE_:
            return 1; /* unequal */
        case STRING_:
            name2 = nodevalue(second).str;
            goto cmpstr;
        default:
            name2 = nickname(type2);
            goto cmpstr;
        }
        break;
    }
cmpdbl:
    dbl = dbl1 - dbl2;
    return dbl < 0 ? -1 : dbl > 0;
cmpstr:
    num = strcmp(name1, name2);
    return num < 0 ? -1 : num > 0;
cmpnum:
    num = num1 - num2;
    return num < 0 ? -1 : num > 0;
}
#endif
