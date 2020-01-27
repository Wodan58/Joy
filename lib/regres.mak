regres: gcd24_54 modtst test jp-reprodtst mtrtst flatjoy jp-church reptst symtst jp-joytst plgtst lsptst laztst jp-nestrec grmtst

gcd24_54:
	../joy gcd 24 54 >gcd.out

modtst:
	../joy $@.joy >$@.out

test:
	../joy $@.joy >$@.out

joytut:
	../joy $@.inp >$@.out

jp-reprodtst:
	../joy $@.joy >$@.out

mtrtst:
	../joy $@.joy >$@.out

flatjoy:
	../joy $@.joy >$@.out

jp-church:
	../joy $@.joy >$@.out

reptst:
	../joy $@.joy >$@.out

symtst:
	../joy $@.joy >$@.out

jp-joytst:
	../joy $@.joy >$@.out

plgtst:
	../joy $@.joy >$@.out

lsptst:
	../joy $@.joy >$@.out

laztst:
	../joy $@.joy >$@.out

jp-nestrec:
	../joy $@.joy >$@.out

grmtst:
	../joy $@.joy >$@.out
