/*
    module  : format.c
    version : 1.7
    date    : 06/21/24
*/
#ifndef FORMAT_C
#define FORMAT_C

/**
OK 1760  format  :  N C I J  ->  S
S is the formatted version of N in mode C
('d or 'i = decimal, 'o = octal, 'x or
'X = hex with lower or upper case letters)
with maximum width I and minimum width J.
*/
void format_(pEnv env)
{
    int width, prec, leng;
    char spec, format[MAXNUM], *result;

    FOURPARAMS("format");
    INTEGERS2("format");
    prec = nodevalue(env->stck).num;
    POP(env->stck);
    width = nodevalue(env->stck).num;
    POP(env->stck);
    CHARACTER("format");
    spec = (char)nodevalue(env->stck).num;
    POP(env->stck);
    CHECKFORMAT(spec, "format");
    strcpy(format, "%*.*lld");
    format[6] = spec;
    NUMERICTYPE("format");
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).num) + 1;
#ifdef NOBDW
    result = malloc(leng + 1);
#else
    result = GC_malloc_atomic(leng + 1);
#endif
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).num);
    UNARY(STRING_NEWNODE, result);
#ifdef NOBDW
    free(result);
#endif
}
#endif
