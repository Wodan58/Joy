/*
    module  : formatf.c
    version : 1.6
    date    : 08/18/23
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
PRIVATE void formatf_(pEnv env)
{
    int width, prec, leng;
    char spec, format[6], *result;

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
    result = GC_malloc_atomic(leng + 1);
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).dbl);
    UNARY(STRING_NEWNODE, result);
}
#endif
