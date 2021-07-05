/*
    module  : push.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef PUSH_H
#define PUSH_H

#define PUSH(PROCEDURE, CONSTRUCTOR, VALUE)                                    \
    PRIVATE void PROCEDURE(pEnv env) { NULLARY(CONSTRUCTOR, VALUE); }
#endif
