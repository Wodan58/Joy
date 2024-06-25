/*
    module  : formatf.c
    version : 1.9
    date    : 06/21/24
*/
#ifndef FORMATF_C
#define FORMATF_C

/**
OK 1770  formatf  :  F C I J  ->  S
S is the formatted version of F in mode C
('e or 'E = exponential, 'f = fractional,
'g or G = general with lower or upper case letters)
with maximum width I and precision J.
*/
void formatf_(pEnv env)
{
    int width, prec, leng;
    char spec, format[MAXNUM], *result;

    FOURPARAMS("formatf");
    INTEGERS2("formatf");
    prec = nodevalue(env->stck).num;
    POP(env->stck);
    width = nodevalue(env->stck).num;
    POP(env->stck);
    CHARACTER("formatf");
    spec = (char)nodevalue(env->stck).num;
    POP(env->stck);
    CHECKFORMATF(spec, "formatf");
    strcpy(format, "%*.*g");
    format[4] = spec;
    FLOAT("formatf");
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).dbl) + 1;
#ifdef NOBDW
    result = malloc(leng + 1);
#else
    result = GC_malloc_atomic(leng + 1);
#endif
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).dbl);
    UNARY(STRING_NEWNODE, result);
#ifdef NOBDW
    free(result);
#endif
}
#endif
