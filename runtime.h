/*
 *  module  : runtime.h
 *  version : 1.1
 *  date    : 09/16/24
 */
#define FLOATABLE							\
    (nodetype(env->stck) == INTEGER_ || nodetype(env->stck) == FLOAT_)
#define FLOATABLE2							\
    ((nodetype(env->stck) == FLOAT_					\
    && nodetype(nextnode1(env->stck)) == FLOAT_)			\
    || (nodetype(env->stck) == FLOAT_					\
    && nodetype(nextnode1(env->stck)) == INTEGER_)			\
    || (nodetype(env->stck) == INTEGER_					\
    && nodetype(nextnode1(env->stck)) == FLOAT_))
#define FLOATVAL							\
    (nodetype(env->stck) == FLOAT_					\
    ? nodevalue(env->stck).dbl						\
    : (double)nodevalue(env->stck).num)
#define FLOATVAL2							\
    (nodetype(nextnode1(env->stck)) == FLOAT_				\
    ? nodevalue(nextnode1(env->stck)).dbl				\
    : (double)nodevalue(nextnode1(env->stck)).num)
#define FLOAT_U(OPER)							\
    if (FLOATABLE)							\
    { UNARY(FLOAT_NEWNODE, OPER(FLOATVAL)); return; }
#define FLOAT_P(OPER)							\
    if (FLOATABLE2)							\
    { BINARY(FLOAT_NEWNODE, OPER(FLOATVAL2, FLOATVAL)); return; }
#define FLOAT_I(OPER)							\
    if (FLOATABLE2)							\
    { BINARY(FLOAT_NEWNODE, (FLOATVAL2)OPER(FLOATVAL)); return; }

#ifndef NCHECK
#define ONEPARAM(NAME)							\
    if (!env->stck)							\
    execerror("one parameter", NAME)
#define TWOPARAMS(NAME)							\
    if (!env->stck || !nextnode1(env->stck))				\
    execerror("two parameters", NAME)
#define THREEPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck) || !nextnode2(env->stck))	\
    execerror("three parameters", NAME)
#define FOURPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck)				\
    || !nextnode2(env->stck) || !nextnode3(env->stck))			\
    execerror("four parameters", NAME)
#define FIVEPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck) || !nextnode2(env->stck)	\
    || !nextnode3(env->stck) || !nextnode4(env->stck))			\
    execerror("five parameters", NAME)
#define ONEQUOTE(NAME)							\
    if (nodetype(env->stck) != LIST_)					\
    execerror("quotation as top parameter", NAME)
#define TWOQUOTES(NAME)							\
    ONEQUOTE(NAME);							\
    if (nodetype(nextnode1(env->stck)) != LIST_)			\
    execerror("quotation as second parameter", NAME)
#define THREEQUOTES(NAME)						\
    TWOQUOTES(NAME);							\
    if (nodetype(nextnode2(env->stck)) != LIST_)			\
    execerror("quotation as third parameter", NAME)
#define FOURQUOTES(NAME)						\
    THREEQUOTES(NAME);							\
    if (nodetype(nextnode3(env->stck)) != LIST_)			\
    execerror("quotation as fourth parameter", NAME)
#define SAME2TYPES(NAME)						\
    if (nodetype(env->stck) != nodetype(nextnode1(env->stck)))		\
    execerror("two parameters of the same type", NAME)
#define STRING(NAME)							\
    if (nodetype(env->stck) != STRING_)					\
    execerror("string", NAME)
#define STRING2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != STRING_)			\
    execerror("string as second parameter", NAME)
#define INTEGER(NAME)							\
    if (nodetype(env->stck) != INTEGER_)				\
    execerror("integer", NAME)
#define POSITIVEINTEGER(n, NAME)					\
    if (n < 0)								\
    execerror("non-negative integer", NAME)
#define INTEGER2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != INTEGER_)			\
    execerror("integer as second parameter", NAME)
#define CHARACTER(NAME)							\
    if (nodetype(env->stck) != CHAR_)					\
    execerror("character", NAME)
#define INTEGERS2(NAME)							\
    if (nodetype(env->stck) != INTEGER_					\
    || nodetype(nextnode1(env->stck)) != INTEGER_)			\
    execerror("two integers", NAME)
#define NUMERICTYPE(NAME)						\
    if (nodetype(env->stck) != INTEGER_ && nodetype(env->stck) != CHAR_	\
    && nodetype(env->stck) != BOOLEAN_)					\
    execerror("numeric", NAME)
#define NUMERIC2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != INTEGER_			\
    && nodetype(nextnode1(env->stck)) != CHAR_)				\
    execerror("numeric second parameter", NAME)
#define CHECKNUMERIC(NODE, NAME)					\
    if (nodetype(NODE) != INTEGER_)					\
    execerror("numeric list", NAME)
#define FLOAT(NAME)							\
    if (!FLOATABLE)							\
    execerror("float or integer", NAME);
#define FLOAT2(NAME)							\
    if (!(FLOATABLE2							\
    || (nodetype(env->stck) == INTEGER_					\
    && nodetype(nextnode1(env->stck)) == INTEGER_)))			\
    execerror("two floats or integers", NAME)
#define ISFILE(NAME)							\
    if (nodetype(env->stck) != FILE_ || !nodevalue(env->stck).fil)	\
    execerror("file", NAME)
#define CHECKZERO(NAME)							\
    if (nodevalue(env->stck).num == 0)					\
    execerror("non-zero operand", NAME)
#define CHECKDIVISOR(NAME)						\
    if ((nodetype(env->stck) == FLOAT_ && nodevalue(env->stck).dbl == 0.0) \
    || (nodetype(env->stck) == INTEGER_ && nodevalue(env->stck).num == 0)) \
    execerror("non-zero divisor", "divide");
#define LIST(NAME)							\
    if (nodetype(env->stck) != LIST_)					\
    execerror("list", NAME)
#define LIST2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != LIST_)			\
    execerror("list as second parameter", NAME)
#define USERDEF(NAME)							\
    if (nodetype(env->stck) != USR_)					\
    execerror("user defined symbol", NAME)
#define USERDEF2(NODE, NAME)						\
    if (nodetype(NODE) != USR_)						\
    execerror("user defined symbol", NAME)
#define CHECKLIST(OPR, NAME)						\
    if (OPR != LIST_)							\
    execerror("internal list", NAME)
#define CHECKSETMEMBER(NODE, NAME)					\
    if ((nodetype(NODE) != INTEGER_ && nodetype(NODE) != CHAR_)		\
    || nodevalue(NODE).num < 0 || nodevalue(NODE).num >= SETSIZE)	\
    execerror("small numeric", NAME)
#define CHECKCHARACTER(NODE, NAME)					\
    if (nodetype(NODE) != CHAR_)					\
    execerror("character", NAME)
#define CHECKEMPTYSET(SET, NAME)					\
    if (SET == 0)							\
    execerror("non-empty set", NAME)
#define CHECKEMPTYSTRING(STRING, NAME)					\
    if (*STRING == '\0')						\
    execerror("non-empty string", NAME)
#define CHECKEMPTYLIST(LIST, NAME)					\
    if (!LIST)								\
    execerror("non-empty list", NAME)
#define CHECKSTACK(NAME)						\
    if (!env->stck)							\
    execerror("non-empty stack", NAME)
#define CHECKVALUE(NAME)						\
    if (!env->stck)							\
    execerror("value to push", NAME)
#define CHECKNAME(STRING, NAME)						\
    if (!STRING || *STRING)						\
    execerror("valid name", NAME)
#define CHECKFORMAT(SPEC, NAME)						\
    if (!strchr("dioxX", SPEC))						\
    execerror("one of: d i o x X", NAME)
#define CHECKFORMATF(SPEC, NAME)					\
    if (!strchr("eEfgG", SPEC))						\
    execerror("one of: e E f g G", NAME)
#define POSITIVEINDEX(INDEX, NAME)					\
    if ((nodetype(INDEX) != INTEGER_ &&	nodetype(INDEX) != BOOLEAN_) || \
    nodevalue(INDEX).num < 0) execerror("non-negative integer", NAME)
#define INDEXTOOLARGE(NAME) execerror("smaller index", NAME)
#define BADAGGREGATE(NAME) execerror("aggregate parameter", NAME)
#define BADDATA(NAME) execerror("different type", NAME)
#else
#define ONEPARAM(NAME)
#define TWOPARAMS(NAME)
#define THREEPARAMS(NAME)
#define FOURPARAMS(NAME)
#define FIVEPARAMS(NAME)
#define ONEQUOTE(NAME)
#define TWOQUOTES(NAME)
#define THREEQUOTES(NAME)
#define FOURQUOTES(NAME)
#define SAME2TYPES(NAME)
#define STRING(NAME)
#define STRING2(NAME)
#define INTEGER(NAME)
#define POSITIVEINTEGER(NAME)
#define INTEGER2(NAME)
#define CHARACTER(NAME)
#define INTEGERS2(NAME)
#define NUMERICTYPE(NAME)
#define NUMERIC2(NAME)
#define CHECKNUMERIC(NODE, NAME)
#define FLOAT(NAME)
#define FLOAT2(NAME)
#define ISFILE(NAME)
#define CHECKZERO(NAME)
#define CHECKDIVISOR(NAME)
#define LIST(NAME)
#define LIST2(NAME)
#define USERDEF(NAME)
#define USERDEF2(NODE, NAME)
#define CHECKLIST(OPR, NAME)
#define CHECKSETMEMBER(NODE, NAME)
#define CHECKCHARACTER(NODE, NAME)
#define CHECKEMPTYSET(SET, NAME)
#define CHECKEMPTYSTRING(STRING, NAME)
#define CHECKEMPTYLIST(LIST, NAME)
#define CHECKSTACK(NAME)
#define CHECKVALUE(NAME)
#define CHECKNAME(STRING, NAME)
#define CHECKFORMAT(SPEC, NAME)
#define CHECKFORMATF(SPEC, NAME)
#define POSITIVEINDEX(INDEX, NAME)
#define INDEXTOOLARGE(NAME)
#define BADAGGREGATE(NAME)
#define BADDATA(NAME)
#endif

#define DMP nodevalue(env->dump).lis
#define DMP1 nodevalue(env->dump1).lis
#define DMP2 nodevalue(env->dump2).lis
#define DMP3 nodevalue(env->dump3).lis
#define DMP4 nodevalue(env->dump4).lis
#define DMP5 nodevalue(env->dump5).lis
#define SAVESTACK env->dump = LIST_NEWNODE(env->stck, env->dump)
#define SAVED1 DMP
#define SAVED2 nextnode1(DMP)
#define SAVED3 nextnode2(DMP)
#define SAVED4 nextnode3(DMP)
#define SAVED5 nextnode4(DMP)
#define SAVED6 nextnode5(DMP)

#define GNULLARY(NODE)							\
    env->stck = newnode2(env, NODE, env->stck)
#define GUNARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode1(env->stck))
#define GBINARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode2(env->stck))
#define GTERNARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode3(env->stck))

#include "src/andorxor.h"
#include "src/bfloat.h"
#include "src/comprel.h"
#include "src/comprel2.h"
#include "src/cons_swons.h"
#include "src/dipped.h"
#include "src/fileget.h"
#include "src/help.h"
#include "src/if_type.h"
#include "src/inhas.h"
#include "src/maxmin.h"
#include "src/n_ary.h"
#include "src/of_at.h"
#include "src/ordchr.h"
#include "src/plusminus.h"
#include "src/predsucc.h"
#include "src/push.h"
#include "src/someall.h"
#include "src/type.h"
#include "src/ufloat.h"
#include "src/unmktime.h"
#include "src/usetop.h"
