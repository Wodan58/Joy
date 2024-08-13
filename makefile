#
#   module  : %M%
#   version : %I%
#   date    : %G%
#
.PRECIOUS: main.s
.SUFFIXES:
.SUFFIXES: .asm .c .o .s

#
#	This builds a Windows x64 binary.
#
CC = gcc
CF = -DNOBDW -I../ASM/include -O3 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-sign-compare -Wno-implicit-fallthrough -Werror -mno-sse -mno-red-zone -fno-stack-protector -fpie -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-align-functions -fno-align-loops -fno-align-jumps -fno-align-labels -fno-exceptions -fno-ident -fno-tree-vectorize -fomit-frame-pointer -fverbose-asm -fleading-underscore -S -masm=intel 
LF = -static -nostdlib -Wl,--entry=init -Wl,--build-id=none -Wl,-Map=joy.map -L../ASM/lib -lcrt -L/usr/lib/w32api -lkernel32
CFLAGS = $(CF) -DCOMP="\"$(CF)\"" -DLINK="\"$(LF)\"" -DVERS="\"NOBDW Release 1.0\""
LFLAGS = $(LF)

HDRS = globals.h
OBJS = main.o interp.o scan.o utils.o factor.o module.o optable.o symbol.o \
       undefs.o gc.o

################################################################################

joy:	prep $(OBJS) ../ASM/lib/libcrt.a
	$(CC) -o$@ $(OBJS) $(LFLAGS)

$(OBJS): $(HDRS)

main.asm: main.c
interp.asm: interp.c
scan.asm: scan.c
utils.asm: utils.c
factor.asm: factor.c
module.asm: module.c
optable.asm: optable.c
symbol.asm: symbol.c
undefs.asm: undefs.c
gc.asm: gc.c

################################################################################

prep:
	sh prims.sh .
	sh table.sh .

.asm.o:
	nasm -o$@ -fwin64 -l$*.lst $<

.s.asm:
	../ASM/my_lexer $<
	mv $<.text $@

.c.s:
	$(CC) -o$@ $(CFLAGS) $<

clean:
	rm -f $(OBJS) *.s main.asm interp.asm scan.asm utils.asm factor.asm module.asm optable.asm symbol.asm undefs.asm gc.asm
