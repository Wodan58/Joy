/*
    module  : push2.h
    version : 1.1
    date    : 01/22/24
*/
#ifndef PUSH2_H
#define PUSH2_H

#define PUSH2(PROCEDURE, CONSTRUCTOR, VALUE)				\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	if (env->ignore) {						\
	    env->bucket.num = 0;					\
	    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);	\
	    return;							\
	}								\
	NULLARY(CONSTRUCTOR, VALUE);					\
    }
#endif
