#
#   module  : makefile
#   version : 1.11
#   date    : 01/17/24
#
.POSIX:
.SUFFIXES:

CC = gcc
CF = -DCOPYRIGHT -DNOBDW -O3 -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter
LF = -lm
CFLAGS = $(CF) -DCOMP="\"$(CF)\"" -DLINK="\"$(LF)\"" -DVERS="\"NOBDW Release 1.0\""
HDRS = globals.h
OBJS = main.o interp.o scan.o utils.o factor.o module.o gc.o

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
	SOURCE_DATE_EPOCH=1047920271 $(CC) -o$@ $(CFLAGS) -c $<
