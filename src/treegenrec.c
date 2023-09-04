/*
    module  : treegenrec.c
    version : 1.7
    date    : 09/04/23
*/
#ifndef TREEGENREC_C
#define TREEGENREC_C

/**
OK 2890  treegenrec  :  T [O1] [O2] [C]  ->  ...
T is a tree. If T is a leaf, executes O1.
Else executes O2 and then [[[O1] [O2] C] treegenrec] C.
*/
PRIVATE void treegenrec_(pEnv env)
{ /* T [O1] [O2] [C]        */
    FOURPARAMS("treegenrec");
    THREEQUOTES("treegenrec");
    cons_(env);
    cons_(env);
    _treegenrec_(env);
}
#endif
