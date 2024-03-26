/*
    module  : format.c
    version : 1.5
    date    : 03/21/24
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
    char spec, format[7], *result;

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
    strcpy(format, "%*.*ld");
    format[5] = spec;
    NUMERICTYPE("format");
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).num) + 1;
    result = GC_malloc_atomic(leng + 1);
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).num);
    UNARY(STRING_NEWNODE, result);
}
#endif
