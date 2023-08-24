Joy
===

Build|Linux|Windows|Coverity
---|---|---|---
status|[![GitHub CI build status](https://github.com/Wodan58/Joy/actions/workflows/cmake.yml/badge.svg)](https://github.com/Wodan58/Joy/actions/workflows/cmake.yml)|[![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/github/Wodan58/Joy?branch=master&svg=true)](https://ci.appveyor.com/project/Wodan58/Joy)|[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/14641.svg)](https://scan.coverity.com/projects/wodan58-joy)

This is the NOBDW version of [Joy1](https://github.com/Wodan58/joy1).
The two versions are drifting apart.
[Joy](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thun01.pdf) is a
decent language and needs a ditto
[presentation](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thomas01a.pdf). The original version can be seen [here](https://github.com/Wodan58/joy0).

Build instructions
------------------

    cd build
    cmake ..
    cmake --build .

Changes
-------

Some changes were done that make ~the build environment~ me happy. One new
thing is a portable makefile, following this [advice](http://nullprogram.com/blog/2017/08/20).
There is something different in these sources compared to the ones that can be
downloaded from the archived [original](http://web.archive.org/web/20030602190407/https://www.latrobe.edu.au/philosophy/phimvt/joy.html) and that is the
comments at the start of main.c and of interp.c.
[Manfred von Thun](http://fogus.me/important/von-thun/) really wanted those
comments in main.c, but the newsgroup did not allow attachments, so it was
never published.
As for the comments in interp.c, I added them because I like some version
history. It proves that the language and the implementation was not done over
night.
In fact, according to this [interview](http://archive.vector.org.uk/art10000350)
the implementation in C was started in 1995.

Testing
-------

    cd test2
    for i in *.joy
    do
      ../build/joy $i >$i.out
    done
    grep -l false *.out

There are some false positives.

Bugs
----

Maybe. As this is a modified version of Joy there are some differences
compared to the legacy version. Those differences and any remaining bugs
are copyright © Wodan58.

Comparison
==========

This implementation can be compared with other implementations.

Time
----

The question is how many fibs can be calculated with the slow recursive
definition: `[small] [] [pred dup pred] [+] binrec.`. In the case of Moy
and Coy the definition starts with: `[dup small]`. The time limit is 60
seconds.

Implementation|Number of fibs
--------------|--------------
[joy0](https://github.com/Wodan58/joy0)|41
[Joy](https://github.com/Wodan58/Joy)|39
[Moy](https://github.com/Wodan58/Moy)|38
[joy1](https://github.com/Wodan58/joy1)|37
[42minjoy](https://github.com/Wodan58/42minjoy)|37
[Coy](https://github.com/Wodan58/Coy)|30

The original version comes out as the winner. Moy has a particularly slow
function calling mechanism and is surprisingly not the slowest. The version
that is closest to the version that could be downloaded from the original
website, `joy1`, is at the same performance level as the antique `42minjoy`.

Space
-----

The question is how many integers can be collected in a list, using the
`from-to-list` from `agglib.joy`.

Implementation|Number of integers
--------------|------------------
[Moy](https://github.com/Wodan58/Moy)|100000
[joy1](https://github.com/Wodan58/joy1)|72000
[Coy](https://github.com/Wodan58/Coy)|43000
[Joy](https://github.com/Wodan58/Joy)|42000
[joy0](https://github.com/Wodan58/joy0)|32295

Moy comes out on top. The other versions fail because of stack overflow.
In fact, because Moy does recursion in a stackless fashion, the size of the
list is only limited by available memory.
