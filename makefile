#
#   module  : makefile
#   version : 1.7
#   date    : 10/12/23
#
.SUFFIXES:
.SUFFIXES: .c .o

CC     = gcc
CFLAGS = -DCOPYRIGHT -DNOBDW -I. -O3 -Wall -Wextra -Wpedantic \
	 -Wno-unused-parameter -Werror -DJVERSION="\"NOBDW Release 1.0\""
HDRS   = globals.h
OBJS   = interp.o scan.o utils.o main.o factor.o module.o gc.o

joy:	prep $(OBJS)
	$(CC) -o$@ $(OBJS) -lm

$(OBJS): $(HDRS)

prep:
	sh prims.sh .
	sh table.sh .

clean:
	rm -f $(OBJS) builtin.* table.c

.c.o:
	SOURCE_DATE_EPOCH=1047920271 $(CC) -o$@ $(CFLAGS) -c $<
