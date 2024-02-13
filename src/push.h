/*
    module  : push.h
    version : 1.2
    date    : 01/22/24
*/
#ifndef PUSH_H
#define PUSH_H

#define PUSH(PROCEDURE, CONSTRUCTOR, VALUE)				\
    PRIVATE void PROCEDURE(pEnv env) { NULLARY(CONSTRUCTOR, VALUE); }
#endif
