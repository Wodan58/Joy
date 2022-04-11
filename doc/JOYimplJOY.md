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

The program is defective. The first condition should read `[pop null]` instead
of `[null]`. Thanks to MEMORYMAX, running this program with the NOBDW version
results in the following error message:

    run time error: memory needed for copying.

Needless to say, increasing MEMORYMAX and recompiling does not help in this
case. The BDW version allocates all physical memory, all swap memory and then
has to wait until the OOM killer comes along.

More comparisons
================

While testing all builtins I noticed that the versions of Joy react differently
to the following testfile:

    "\07".
    { true }.
    [ ( ].
    [ } ].
    123.

The 123 at the end is present to see whether the entire file is processed.
Here is the response from Joy:

    "\07".
        ^
	    digit expected
    { true }.
         ^
	    small numeric expected in set
    {}
    [ ( ].
      ^
	    '(' not implemented
    [ ( ].
         ^
	    ']' expected
    [ } ].
      ^
	    ']' expected
    []
    [[]]
    123

The error detection from Joy, and also joy1, is excellent. joy1 implements the
same language as Joy, but the output is different:

    "\07".
        ^
	    digit expected
    { true }.
         ^
	    small numeric expected in set
    {}
    [ ( ].
      ^
	    '(' not implemented
    [ ( ].
         ^
	    ']' expected
    [ } ].
      ^
	    ']' expected
    exeterm: attempting to execute bad node
    123

Handling of memory is different in joy1 compared to Joy and also exeterm is a
bit different: joy1 does not use conts. All the same it is surprising. Moy and
Coy implement a language that is a little different and they do so with a
different code base, so some differences are expected. Here is the output from
Moy:

    syntax error in file t.joy line 6 : "\07".
    syntax error in file t.joy line 7 : { true }.
    syntax error in file t.joy line 8 : [ ( ].
    syntax error in file t.joy line 9 : [ } ].
    123 

Errors are sent to stderr, by the way. The text "syntax error" comes from yacc.
And here is the output from Coy:

    "\07".
       ^
	    digit expected
    ""
    { true }.
         ^
	    syntax error
    { true }.
          ^
	    syntax error
    [ ( ].
      ^
	    syntax error
    [ ( ].
       ^
	    syntax error
    [ } ].
     ^
	    syntax error
    [ } ].
       ^
	    syntax error
    123

Coy has a handwritten lexical analyzer, making the first error possible. These
messages may change in the future, allowing for more compatibility with Joy and
joy1. As for now, this test file can be used to distinguish between the four
repositories. After all, they all produce as output `joy`. Version numbers and
compilation dates are not very reliable as a way to distinguish the different
versions of Joy.

Summary
=======

This updated version of Joy is slowly drifting away from the legacy version.

- C89 support has been dropped.
