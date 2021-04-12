 ![](Wynn.PNG)
==============

Introduction
============

This presents some notes about the technicalities of this Joy implementation.
Most of the remarks from joy1, the BDW version. also apply here.

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

Summary
=======

This updated version of Joy is slowly drifting away from the legacy version.

- C89 support has been dropped.
