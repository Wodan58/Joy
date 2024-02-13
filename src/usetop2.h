/*
    module  : usetop2.h
    version : 1.1
    date    : 01/22/24
*/
#ifndef USETOP2_H
#define USETOP2_H

#define USETOP2(PROCEDURE, NAME, TYPE, BODY)				\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	ONEPARAM(NAME);							\
	TYPE(NAME);							\
	BODY;								\
	POP(env->stck);							\
    }
#endif
