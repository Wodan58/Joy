Joy
---

This is a distribution of [Joy1](https://github.com/Wodan58/joy1), without
the libraries. All modifications done in that fork make it difficult to see
what the original version was. Joy is a fine language and needs a decent
presentation.

Changes
-------

All changes done in Joy1 have been undone. The only new thing is a portable
makefile, following the advice from [here](nullprogram.com/blog/2017/08/20).
Even so, there is something different in these sources compared to the ones
that you can download from the original [site](www.latrobe.edu.au/humanities/research/research-projects/past-projects/joy-programming-language) and that is
the comments at the start of main.c and of interp.c. Manfred von Thun really
wanted those comments in main.c, but the [newsgroup](https://groups.yahoo.com/neo/groups/concatenative/info) did not allow attachments, so it was never
realized. As for the comments in interp.c, I added them because I like some
version history. It proves that the language and the implementation was not
done during one night. In fact, according to this [interview](https://www.nsl.com/papers/interview.htm) the implementation in C was started in 1995.

Installation
------------

The [BDW garbage collector](https://github.com/ivmai/bdwgc) needs to be
installed and compiled separately, although Joy can operate without.
The makefile assumes that the bdwgc directory is located below the Joy
directory. It is not necessary to make a copy; you can create a symlink,
like this: ln -s /usr/local/src/bdwgc bdwgc.
