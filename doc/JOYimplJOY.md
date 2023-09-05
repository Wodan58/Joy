 ![](Wynn.PNG)
==============

Introduction
============

This presents a note about the technicalities of this Joy implementation. Most
of the remarks from joy1, the BDW version, also apply here.

Recent changes
==============

Both the memory area and the symbol table can grow when needed.

This repository is referred to from the wikipedia page about Joy. That page
contains this text: "Its library routines mirror those of ISO C, though the
current implementation is not easily extensible with functions written in C".
This implementation makes adding a function easier. All it takes is to write
the function and relocate the source code in the src-directory. The build
system takes it from there. Not a general FFI, but probably good enough.

Comparison
==========

Comparing the different versions of joy is now the task of
[joyid.joy](https://github.com/Wodan58/Joy/blob/master/doc/joyid.joy).

Summary
=======

This updated version of Joy tries to stay as close to the legacy version as
possible.
