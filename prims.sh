#
#   module  : prims.sh
#   version : 1.6
#   date    : 05/01/24
#
#   Generate builtin.c and builtin.h
#   The directory needs to be given as parameter.
#
echo checking builtin.c and builtin.h
todo=0
ls $1/src/*.c | sed 's/^/#include "/;s/$/"/' >prims.tmp
if [ ! -f $1/builtin.c -o ! -f $1/builtin.h ]
then
  echo creating builtin.c and builtin.h
  todo=1
else
  diff $1/builtin.c prims.tmp
  if [ $? -eq 0 ]
  then
    echo builtin.c and builtin.h are up-to-date
    rm prims.tmp
  else
    echo updating builtin.c and builtin.h
    todo=1
  fi
fi
if [ $todo -eq 1 ]
then
  rm -f $1/builtin.c $1/builtin.h
  mv prims.tmp $1/builtin.c
  sed 's/.*\//void /;s/\..*/_(pEnv env);/' <$1/builtin.c >$1/builtin.h
  touch $1/optable.c
fi
