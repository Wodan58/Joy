/*
    module  : push.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef PUSH_H
#define PUSH_H

#define PUSH(PROCEDURE, CONSTRUCTOR, VALUE)				\
    void PROCEDURE(pEnv env) { NULLARY(CONSTRUCTOR, VALUE); }
#endif
