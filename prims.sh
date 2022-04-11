#
#   module  : prims.sh
#   version : 1.3
#   date    : 04/11/22
#
#   Generate builtin.c and builtin.h
#   The directory needs to be given as parameter.
#
echo checking builtin.c and builtin.h
todo=0
ls -1Q $1/src/*.c | sed 's/^/#include /' >prims.tmp
if [ ! -f $1/builtin.c -o ! -f $1/builtin.h ]
then
  echo creating builtin.c and builtin.h
  todo=1
else
  diff prims.tmp $1/builtin.c
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
  touch $1/interp.c
fi
