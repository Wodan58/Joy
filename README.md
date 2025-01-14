Joy
===

Build|Linux|Windows|Coverity
---|---|---|---
status|[![GitHub CI build status](https://github.com/Wodan58/Joy/actions/workflows/cmake.yml/badge.svg)](https://github.com/Wodan58/Joy/actions/workflows/cmake.yml)|[![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/github/Wodan58/Joy?branch=master&svg=true)](https://ci.appveyor.com/project/Wodan58/Joy)|[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/14641.svg)](https://scan.coverity.com/projects/wodan58-joy)

This is the NOBDW version of [Joy1](https://github.com/Wodan58/joy1).
[Joy](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thun01.pdf) is a
decent language and needs a ditto
[presentation](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thomas01a.pdf). The original version can be seen [here](https://github.com/Wodan58/joy0).

Changes
-------

Some changes were done that make ~the build environment~ me happy. One new
thing is a portable makefile, following this [advice](http://nullprogram.com/blog/2017/08/20).
There is something different in these sources compared to the ones that can be
downloaded from the archived [original](http://web.archive.org/web/20030602190407/https://www.latrobe.edu.au/philosophy/phimvt/joy.html) and that is the
comments at the start of main.c and of interp.c.
[Manfred von Thun](http://fogus.me/important/von-thun/) really wanted those
comments in main.c, but the newsgroup did not allow attachments, so it was
never published as such.
As for the comments in interp.c, I added them because I like some version
history. It proves that the language and the implementation was not done over
night.
In fact, according to this [interview](http://archive.vector.org.uk/art10000350)
the implementation in C was started in 1995. And according to the
[bibliography](https://wodan58.github.io/joybibl.html) the first papers about
Joy date back to 1994.

Build instructions
------------------

    rm -rf build
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..
    cmake --build .

    cp ../lib/usrlib.joy ~
    mkdir ~/usrlib
    cp ../lib/* ~/usrlib

Then manually change the path to inilib.joy in ~/usrlib.joy from "../lib" to
"usrlib".

Build with MSVC
---------------

    cmake --build . --config Release
    copy Release\joy.exe

    copy ..\lib\usrlib.joy %HOMEPATH%
    mkdir %HOMEPATH%\usrlib
    copy ..\lib\*.* %HOMEPATH%\usrlib

Then manually change the path to inilib.joy in %HOMEPATH%\usrlib.joy from
"../lib" to "usrlib".

Running
-------

    joy -h

gives an overview of available options.

Testing
-------

    cd ../test2
    for i in *.joy
    do
      ../build/joy $i >$i.out
    done
    grep -l false *.out

There are some false positives. The test files assume that usrlib.joy was
loaded successfully.

See also
--------

Implementation|Dependencies
--------------|------------
[42minjoy](https://github.com/Wodan58/42minjoy)|
[joy0](https://github.com/Wodan58/joy0)|
[joy1](https://github.com/Wodan58/joy1)|[BDW garbage collector](https://github.com/ivmai/bdwgc)
[Foy](https://github.com/Wodan58/Foy)|[BDW garbage collector](https://github.com/ivmai/bdwgc)
[Moy](https://github.com/Wodan58/Moy)|[BDW garbage collector](https://github.com/ivmai/bdwgc) and [Lex & Yacc](https://sourceforge.net/projects/winflexbison/files/win_flex_bison-latest.zip)

Documentation|Notes
-------------|-----
[Legacy Docs](https://wodan58.github.io)|Link in About section
[User Manual](https://wodan58.github.io/j09imp.html)|
[Comparison](https://github.com/Wodan58/HET/blob/master/doc/FIB.md)|Speed comparison
[Main Manual (PDF)](https://github.com/Wodan58/G3/blob/master/JOP.pdf)|3rd edition
[Academic Article (PDF)](https://sol.sbc.org.br/index.php/sblp/article/view/30253/30060)|Conversion to/from Combinatory Calculus
