.POSIX:
.SUFFIXES:

CC = gcc
CFLAGS = -DGC_BDW -Ibdwgc/include -Ofast -Wall -Wextra
LDLIBS = -Lbdwgc -lgc-lib -lm
OBJECTS = interp.o main.o scan.o utils.o

joy.exe: $(OBJECTS)
	$(CC) -o$@ $(OBJECTS) $(LDLIBS)

$(OBJECTS): globals.h

clean:
	rm -f joy.exe *.o

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<
