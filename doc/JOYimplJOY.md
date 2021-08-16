 ![](Wynn.PNG)
==============

Introduction
============

This presents some notes about the technicalities of this Joy implementation.
Most of the remarks from joy1, the BDW version, also apply here.

Recent changes
==============

Node* changed to Index: a large change, but only technical. No change in
functionality.

gc.c added: this adds garbage collection for strings. That was missing in the
NOBDW version of Joy and is really needed in order to arrive at a complete
solution. Otherwise an application might be leaking memory.

khash.h added: gc.c uses the hashing table from khash.h. Now that this file is
present it might be used for the symbol table as well. Same as in joy1.

kvec.h added: The symbol table can grow when needed.

Comparison
==========

Comparing this implementation with joy1, it seems that only the choice of
garbage collector is different. The builtins in the src-directory require more
work than in de BDW version, because each allocation must be attached to one of
the roots for garbage collection before another allocation might occur. This
makes the BDW version easier to extend with new builtins. Performance between
the BDW version and the NOBDW version is similar. Only the MINBDW version that
removes the extra dependency on the BDW collector is slower.

This version also uses MEMORYMAX. While this may seem a disadvantage - try
reading a 10 kilobyte file or build a list with 10000 integers - there is also
an advantage. Consider the following program:

    DEFINE
        cnr-ack ==
            [ [ [null]  [popd succ] ]
            [ [null]  [pop pred 1]  [] ]
            [ [[dup pred swap] dip pred]  []  [] ] ]
        condnestrec.

    3 4 cnr-ack.

The program is defective. The first condition should read [pop null] instead of
[null]. Thanks to MEMORYMAX, running this program with the NOBDW version
results in the following error message:

run time error: memory needed for copying.

Needless to say, increasing MEMORYMAX and recompiling does not help in this
case. The BDW version allocates all physical memory, all swap memory and then
has to wait until the OOM killer comes along.

Summary
=======

This updated version of Joy is slowly drifting away from the legacy version.

- C89 support has been dropped.
