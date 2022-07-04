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
work than in the BDW version, because each allocation must be attached to one of
the roots for garbage collection before another allocation might occur. This
makes the BDW version easier to extend with new builtins. Performance between
the BDW version and the NOBDW version is similar. Only the MINBDW version that
removes the extra dependency on the BDW collector is slower.

Consider the following program:

    DEFINE
        cnr-ack ==
            [ [ [null]  [popd succ] ]
            [ [null]  [pop pred 1]  [] ]
            [ [[dup pred swap] dip pred]  []  [] ] ]
        condnestrec.

    3 4 cnr-ack.

The program is defective. The first condition should read `[pop null]` instead
of `[null]`. Running this program with the NOBDW version results in the
following error message:

    run time error: memory needed for copying.

Joy1 responds differently. The MINBDW version does not give any error message.
It simply runs forever. The BDW version responds with the following error
messages:

    GC Warning: Out of Memory! Heap size: 1940 MiB. Returning NULL!
    run time error: memory needed for allocator.

More comparisons
================

While testing all builtins I noticed that the versions of Joy react differently
to the following testfile:

    "\07".
    { true }.
    [ ( ].
    [ } ].

The response from joy0, with commandline `./joy <test17.joy`:

    JOY  -  compiled at 11:02:05 on Jun 13 2022 
    "\07".
        ^
	    digit expected
    { true }.
         ^
	    numeric expected in set
    {}
    [ ( ].
      ^
	    a factor cannot begin with this symbol
    [[]]
    [ } ].
      ^
	    ']' expected
    []
    time:  54 CPU,  0 gc (= 0%)

Here is the response from the original Joy:

    "\07".
        ^
	    digit expected
    { true }.
         ^
	    numeric expected in set
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
    [ } ].
        ^    END or period '.' expected
    [[]]

The error detection from the original Joy is excellent. The new versions, joy1
and Joy implement the same language and the output is now also the same:

    "\07".
        ^
	    digit expected
    { true }.
         ^
	    small numeric expected in set
    [ ( ].
      ^
	    '(' not implemented
    [ ( ].
         ^
	    ']' expected
    [ } ].
      ^
	    ']' expected

In case of an error the stack is cleared. Handling of memory is different in
joy1 compared to Joy and also exeterm is a bit different: joy1 does not use
conts. It is good to know that these versions agree about the result.

It is still possible to distinguish between Joy and joy1 with the help of the
following program:

    [1 2 3] -1 drop..

In case of a runtime error, Joy clears the stack and joy1 doesn't. So, the
output of joy1 will be:

    run time error: non-negative integer needed for drop
    -1

whereas Joy will only print the runtime error and not the -1.

Moy and Coy implement the same language with a different code base, so some
differences can be expected. Here is the output from Moy:

    syntax error: "\07".
    syntax error: { true }.
    syntax error: [ ( ].
    syntax error: [ } ].

Errors are sent to stderr, by the way. The text "syntax error" comes from yacc.
And here is the output from Coy:

    "\07".
       ^
	    digit expected
    "\007"
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

Coy has a handwritten lexical analyzer, making the first error possible. These
messages may change in the future, allowing for more compatibility with Joy and
joy1. As for now, this test file can be used to distinguish between the four
repositories. After all, they all produce as output `joy`. Version numbers and
compilation dates are not very reliable as a way to distinguish the different
versions of Joy.

Summary
=======

This updated version of Joy is slowly drifting away from the legacy version.
