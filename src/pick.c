/*
    module  : pick.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef PICK_C
#define PICK_C

/**
OK 3190  pick  :  X Y Z 2  ->  X Y Z X
[EXT] Pushes an extra copy of nth (e.g. 2) item X on top of the stack.
*/
void pick_(pEnv env)
{
    Index item;
    int i, size;

    FOURPARAMS("pick"); /* minimum of four: 0 pick == dup; 1 pick == over */
    INTEGER("pick");
    size = nodevalue(env->stck).num; /* pick up the number */
    POP(env->stck); /* remove top of stack */
    item = env->stck; /* if the stack is too small, the last item is selected */
    for (i = 0; i < size && nextnode1(item); i++) /* top of stack was popped */
	item = nextnode1(item); /* possibly select the last item on the stack */
    GNULLARY(item);
}
#endif
