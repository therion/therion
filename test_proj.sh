#!/bin/bash

# Test linking to multiple versions of the PROJ library.

PREFIX=/opt

set -e
rm -f thproj.o utest-proj.o

for ver in ${1:-4.9.3 5.1.0 5.2.0 6.2.1 6.3.2 7.0.0 7.2.1}
do
  export PROJ_MVER=`echo $ver | sed 's/\..*//'`
  export PROJ_LIBS="$PREFIX/proj-$ver/src/.libs/libproj.a"
  if ((PROJ_MVER == 4)); then PROJ_LIBS+=" -pthread"; fi
  if ((PROJ_MVER >= 6)); then PROJ_LIBS+=" -lsqlite3"; fi
  if ((PROJ_MVER >= 7)); then PROJ_LIBS+=" -ldl -lcurl -ltiff"; fi
  export CXXJFLAGS="-DPROJ_VER=$PROJ_MVER -I$PREFIX/proj-$ver/src"
  if ((PROJ_MVER >= 6)); then
    export PROJ_LIB="$PREFIX/proj-$ver/data";
  else
    export PROJ_LIB="$PREFIX/proj-$ver/nad";
  fi
  tclsh thcsdata.tcl $PREFIX/proj-$ver/nad
  make therion
  make tests
  rm thproj.o utest-proj.o thcsdata.o thcsdata.h
done
