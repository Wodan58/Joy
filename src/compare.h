/*
    module  : %M%
    version : %I%
    date    : %G%
*/
#ifndef COMPARE_H
#define COMPARE_H

PRIVATE double Compare(pEnv env, Index first, Index second, int *error)
{
    *error = 0;
    switch (nodetype(first)) {
    case USR_:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                opername(nodetype(second)));
        }
        break;
    case ANON_FUNCT_:
        switch (nodetype(second)) {
        case USR_:
            break;
        case ANON_FUNCT_:
            return (size_t)nodevalue(first).proc
                - (size_t)nodevalue(second).proc;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case BOOLEAN_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case CHAR_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case INTEGER_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case SET_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            break;
        case SET_:
            return nodevalue(first).set - nodevalue(second).set;
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case STRING_:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(nodevalue(first).str,
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(nodevalue(first).str, nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(nodevalue(first).str, opername(nodetype(second)));
        }
        break;
    case LIST_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case FLOAT_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).dbl - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).dbl - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case FILE_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
            break;
        case FILE_:
            return nodevalue(first).fil - nodevalue(second).fil;
        default:
            break;
        }
        break;
    default:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(opername(nodetype(first)),
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(opername(nodetype(first)), nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(
                opername(nodetype(first)), opername(nodetype(second)));
        }
        break;
    }
    *error = 1;
    return 0;
}
#endif
