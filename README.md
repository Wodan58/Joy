Joy
---

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

    mkdir build
    cd build
    cmake ..
    cmake --build .

or:

    make -f make.nogc

The second build produces joy.com

Changes
-------

Some changes were done that make ~the build environment~ me happy. One new
thing is a portable makefile, following this [advice](http://nullprogram.com/blog/2017/08/20).
There is something different in these sources compared to the ones that can be
downloaded from the original [site](http://www.latrobe.edu.au/humanities/research/research-projects/past-projects/joy-programming-language) and that is the
comments at the start of main.c and of interp.c.
[Manfred von Thun](http://fogus.me/important/von-thun/) really wanted those
comments in main.c, but the newsgroup did not allow attachments, so it was
never published.
As for the comments in interp.c, I added them because I like some version
history. It proves that the language and the implementation was not done over
night.
In fact, according to this [interview](http://archive.vector.org.uk/art10000350) the implementation in C was started in 1995.

Binaries
--------

The binary `joy.com` is a 64-bit executable that should work on most x86_64
systems. The [support vector](http://github.com/jart/cosmopolitan)
at the end of the README.md tells what systems exactly.

Testing
-------

    cd test2
    for i in *.joy
    do
      ../joy.com $i >$i.out
    done
    grep -l false *.out

There are some false positives.

Bugs
----

Yes, and they are copyright (c) Wodan58.
