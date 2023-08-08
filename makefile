#
#   module  : makefile
#   version : 1.1
#   date    : 08/08/23
#
.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS = -DCOPYRIGHT -DNOBDW -I. -O3 -Wall -Wextra -Wno-unused-parameter \
	 -Werror -DJVERSION="\"NOBDW Release 1.0\""
HDRS   = globals.h
OBJS   = interp.o scan.o utils.o main.o factor.o module.o gc.o

joy:	prep $(OBJS)
	$(CC) -o$@ $(OBJS)

$(OBJS): $(HDRS)

prep:
	sh prims.sh .
	sh table.sh .

clean:
	rm -f $(OBJS) builtin.* table.c

.c.o:
	$(CC) -o$@ $(CFLAGS) -c $<
