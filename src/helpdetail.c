/*
    module  : helpdetail.c
    version : 1.2
    date    : 07/20/21
*/
#ifndef HELPDETAIL_C
#define HELPDETAIL_C

/**
2940  helpdetail  :  [ S1 S2 .. ]  ->
Gives brief help on each symbol S in the list.
*/
PRIVATE void helpdetail_(pEnv env)
{
    Index n;
    Entry ent;

    ONEPARAM("HELP");
    LIST("HELP");
    printf("\n");
    n = nodevalue(env->stck).lis;
    while (n != NULL) {
        if (nodetype(n) == USR_) {
            ent = vec_at(env->symtab, nodevalue(n).ent);
            printf("%s  ==\n    ", ent.name);
            writeterm(env, ent.u.body, stdout);
            printf("\n");
            break;
        } else {
            Operator op;
            if ((op = nodetype(n)) == BOOLEAN_)
                op = nodevalue(n).num ? TRUE_ : FALSE_;
            if (op == INTEGER_ && nodevalue(n).num == MAXINT)
                op = MAXINT_;
            printf("%s	:   %s.\n%s\n", optable[(int)op].name,
                optable[(int)op].messg1, optable[(int)op].messg2);
        }
        printf("\n");
        n = nextnode1(n);
    }
    POP(env->stck);
}
#endif
