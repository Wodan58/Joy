#
#   module  : table.sh
#   version : 1.3
#   date    : 04/11/22
#
#   Generate table.c
#   The directory needs to be given as parameter.
#
echo checking table.c
todo=0
if [ ! -f $1/table.c ]
then
  echo creating table.c
  todo=1
else
  for i in $1/src/*.c
  do
    if [ $i -nt $1/table.c ]
    then
      echo updating table.c
      todo=1
      break
    fi
  done
fi
if [ $todo -eq 0 ]
then
  echo table.c is up-to-date
  exit
fi
rm -f $1/table.c
for i in $1/src/*.c
do
  j=`basename $i`
  j=`expr $j : '\(.*\)\.c'`
  sed -n '/\/\*\*/ {
	N
	N
	N
	N
	N
	N
	N
	N
	s/[^\n]*\n\([^ ]*\)[ ]*\([^ ]*\)[ ]*:[ ]*\([^\n]*\)[ \n]*\([^*]*\)\*\/.*/\/* \1 *\/ { @\2@, '$j'_, @\3@, @\4@ },/
	s/\n/\\n/g
	s/"/\\"/g
	s/@/"/g
	P
  }' <$i
done | sort >$1/table.c
touch $1/interp.c
