/*
    module  : usetop.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef USETOP_H
#define USETOP_H

#define USETOP(PROCEDURE, NAME, TYPE, BODY)				\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	TYPE(NAME);							\
	BODY;								\
	POP(env->stck);							\
    }
#endif
