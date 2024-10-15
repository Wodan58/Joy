 ![](Wynn.PNG)
==============

Introduction
============

This presents a note about the technicalities of this Joy implementation.
The remarks in
[joy1](https://github.com/Wodan58/joy1/blob/master/doc/JOYimplJOY.md),
the BDW version, also apply here.

Changes
=======

This repository is referred to from the wikipedia page about Joy. That page
contained this text: "Its library routines mirror those of ISO C, though the
current implementation is not easily extensible with functions written in C."

That remark addresses the lack of a general Foreign Function Interface. Such
FFI would indiscriminately import functions from imperative languages into Joy,
destroying its functional nature.

As an example of such import: if Joy had `putenv`, the environment could be
modified and Joy would suddenly have variables. Joy already has `getenv` but
that one is harmless, as it only reads a value from somewhere.

This implementation makes adding a function easier. All it takes is to write
the function and relocate the source code in the src-directory. The build
system takes it from there.

Conversely, it is also possible to remove builtins, such as filetime.
Filetime is not available when compiling with -D\_\_STDC\_\_.

Comparison
==========

Comparing the different versions of joy is the task of
[joyid.joy](https://github.com/Wodan58/Joy/blob/master/doc/joyid.joy).
