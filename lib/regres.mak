regres: gcd24_54 modtst test joytut alljoy jp-reprodtst mtrtst jp-church flatjoy reptst symtst jp-joytst plgtst lsptst laztst jp-nestrec mandel grmtst
	rm -f joy.out

gcd24_54:
	echo gcd
	./joy gcd 24 54 | diff -w - gcd.out

joytut:
	echo joytut
	./joy $@.inp >joy.out
	sed '/^Monday/d;/^time/d;s/^joytut/joytut quit/' <$@.out | diff -w - joy.out

lsptst:
	echo lsptst
	./joy $@.joy | head -261 >joy.out
	head -261 <$@.out | sed 's/^lisp/lisp quit/' | diff -w - joy.out

jp-joytst:
	echo jp-joytst
	./joy $@.joy | sed '/^warning/d' >joy.out
	sed '/^Thursday/d;/^time/d;/^gc/d' <$@.out | diff -w - joy.out

laztst:
	echo laztst
	./joy $@.joy >joy.out
	sed '/^Friday/d;/^Time/d' <$@.out | diff -w - joy.out

grmtst:
	echo grmtst
	./joy $@.joy >joy.out
	-head -330 $@.out | diff -w - joy.out

modtst:
	echo modtst
	./joy $@.joy | diff -w - $@.out

mtrtst:
	echo mtrtst
	./joy $@.joy | diff -w - $@.out

plgtst:
	echo plgtst
	./joy $@.joy | diff -w - $@.out

symtst:
	echo symtst
	./joy $@.joy | diff -w - $@.out

test:
	echo test
	./joy $@.joy | diff -w - $@.out

reptst:
	echo reptst
	./joy $@.joy | diff -w - $@.out

jp-nestrec:
	echo jp-nestrec
	./joy $@.joy | diff -w - $@.out

jp-reprodtst:
	echo jp-reprodtst
	./joy $@.joy | diff -w - $@.out

flatjoy:
	echo flatjoy
	./joy $@.joy | diff -w - $@.out

jp-church:
	echo jp-church
	./joy $@.joy | diff -w - $@.out

mandel:
	echo mandel
	./joy $@.joy | diff -w - $@.out

alljoy:
	echo alljoy
	./joy $@.joy | diff -w - $@.out
