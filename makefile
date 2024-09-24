#
#   module  : makefile
#   version : 1.14.1.2
#   date    : 09/18/24
#
.POSIX:
.SUFFIXES:

CC = gcc # -pg
CF = -DNOBDW -O3 -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter
LF = -lm
CFLAGS = $(CF) -DCOMP="\"$(CF)\"" -DLINK="\"$(LF)\"" -DVERS="\"NOBDW Release 1.0\""
HDRS = globals.h
OBJS = main.o interp.o scan.o utils.o factor.o module.o optable.o symbol.o \
       undefs.o setraw.o repl.o write.o error.o print.o gc.o

joy:	prep $(OBJS)
	$(CC) -o$@ $(OBJS) $(LF)

$(OBJS): $(HDRS)

prep:
	sh prims.sh .
	sh table.sh .

clean:
	rm -f $(OBJS) builtin.* table.c

.SUFFIXES: .c .o

.c.o:
	$(CC) -o$@ $(CFLAGS) -c $<
