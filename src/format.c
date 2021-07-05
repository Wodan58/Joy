/*
    module  : format.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FORMAT_C
#define FORMAT_C

/**
1760  format  :  N C I J  ->  S
S is the formatted version of N in mode C
('d or 'i = decimal, 'o = octal, 'x or
'X = hex with lower or upper case letters)
with maximum width I and minimum width J.
*/
PRIVATE void format_(pEnv env)
{
    int width, prec;
    char spec, format[7], *result;
#ifdef USE_SNPRINTF
    int leng;
#endif
    FOURPARAMS("format");
    INTEGER("format");
    INTEGER2("format");
    prec = nodevalue(env->stck).num;
    POP(env->stck);
    width = nodevalue(env->stck).num;
    POP(env->stck);
    CHARACTER("format");
    spec = (char)nodevalue(env->stck).num;
    POP(env->stck);
    if (!strchr("dioxX", spec))
        execerror(env, "one of: d i o x X", "format");
    strcpy(format, "%*.*ld");
    format[5] = spec;
    NUMERICTYPE("format");
#ifdef USE_SNPRINTF
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).num) + 1;
    result = GC_malloc_atomic(leng + 1);
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).num);
#else
    result = GC_malloc_atomic(INPLINEMAX); /* should be sufficient */
    sprintf(result, format, width, prec, env->stck->u.num);
#endif
    UNARY(STRING_NEWNODE, result);
}
#endif
