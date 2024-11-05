/*
 *  module  : macros.h
 *  version : 1.2
 *  date    : 10/18/24
 */
#define POP(X) X = nextnode1(X)

#define USR_NEWNODE(u, r)						\
    (env->bucket.ent = u, newnode(env, USR_, env->bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)					\
    (env->bucket.proc = u, newnode(env, ANON_FUNCT_, env->bucket, r))
#define BOOLEAN_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, BOOLEAN_, env->bucket, r))
#define CHAR_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, CHAR_, env->bucket, r))
#define INTEGER_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, INTEGER_, env->bucket, r))
#define SET_NEWNODE(u, r)						\
    (env->bucket.set = u, newnode(env, SET_, env->bucket, r))
#define STRING_NEWNODE(u, r)						\
    (env->bucket.str = u, newnode(env, STRING_, env->bucket, r))
#define LIST_NEWNODE(u, r)						\
    (env->bucket.lis = u, newnode(env, LIST_, env->bucket, r))
#define FLOAT_NEWNODE(u, r)						\
    (env->bucket.dbl = u, newnode(env, FLOAT_, env->bucket, r))
#define FILE_NEWNODE(u, r)						\
    (env->bucket.fil = u, newnode(env, FILE_, env->bucket, r))
#define BIGNUM_NEWNODE(u, r)						\
    (env->bucket.str = u, newnode(env, BIGNUM_, env->bucket, r))

#define NULLARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, env->stck)
#define UNARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode1(env->stck))
#define BINARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode2(env->stck))

#define GNULLARY(NODE)							\
    env->stck = newnode2(env, NODE, env->stck)
#define GUNARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode1(env->stck))
#define GBINARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode2(env->stck))
#define GTERNARY(NODE)							\
    env->stck = newnode2(env, NODE, nextnode3(env->stck))

/*
 * Strings are stored in consecutive nodes in the NOBDW version; the BDW
 * version stores them somewhere else.
 */
#ifdef NOBDW
#define GETSTRING(NODE)		(char *)&nodevalue(NODE)
#else
#define GETSTRING(NODE)		nodevalue(NODE).str
#endif
