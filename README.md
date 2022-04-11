Joy
---

Build|Linux|Windows|Coverity|Coverage|Codecov|Quality|Alerts
---|---|---|---|---|---|---|---
status|[![Travis CI build status](https://app.travis-ci.com/Wodan58/Joy.svg?branch=master)](https://app.travis-ci.com/Wodan58/Joy)|[![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/github/Wodan58/Joy?branch=master&svg=true)](https://ci.appveyor.com/project/Wodan58/Joy)|[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/14641.svg)](https://scan.coverity.com/projects/wodan58-joy)|[![Coverage Status](https://coveralls.io/repos/github/Wodan58/Joy/badge.svg?branch=master)](https://coveralls.io/github/Wodan58/Joy?branch=master)|[![Codecov](https://codecov.io/gh/Wodan58/Joy/branch/master/graph/badge.svg)](https://codecov.io/gh/Wodan58/Joy)|[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/Wodan58/Joy.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Wodan58/Joy/context:cpp)|[![Alerts](https://img.shields.io/lgtm/alerts/g/Wodan58/Joy.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Wodan58/Joy/alerts)

This is the NOBDW version of [Joy1](https://github.com/Wodan58/joy1).
The two versions are drifting apart.
[Joy](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thun01.pdf) is a
decent language and needs a ditto
[presentation](http://www.complang.tuwien.ac.at/anton/euroforth/ef01/thomas01a.pdf). The original version can be seen [here](https://github.com/alekar/joy).

Changes
-------

Some changes were done that make the build environment happy. The only new
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

Installation
------------

    mkdir build
    cd build
    cmake ..
    cmake --build .

or:

    make -f make.nogc

Binaries
--------

The binary `joy.exe` is a 32-bit Windows executable; it was compiled by me in
2005 and should still work under Windows; it uses the BDW garbage collector.
The binary `joy.com` is a 64-bit executable that should work on most x86_64
systems. The [support vector](http://github.com/jart/cosmopolitan/README.md)
tells what systems exactly.
