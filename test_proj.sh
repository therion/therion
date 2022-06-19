#!/bin/bash

# Test linking to multiple versions of the PROJ library.
#
# Usage:
#   test_proj.sh [srcpath] [proj_version]
#
# Arguments:
#   srcpath – the path to therion sources (default: .)
#   proj_version – the version of PROJ to test (default: all versions)

set -e

SRCPATH=${1:-.}
PROJVER=${2:-4.9.3 5.1.0 5.2.0 6.2.1 6.3.2 7.0.0 7.2.1 8.0.0 8.2.1 9.0.0 9.0.1}

PREFIX=$HOME/tmp/ThProj_test
URL=https://download.osgeo.org/proj/proj

if [ "$SRCPATH" = "." ]; then
  MODE="Make"
  rm -f thproj.o utest-proj.o
else
  MODE="CMake"
  rm -f CMakeCache.txt
fi

for ver in $PROJVER
do
  # check the environment; install Proj locally if it's missing
  if [ ! -d "$PREFIX/proj-$ver" ]; then
    echo "$PREFIX/proj-$ver missing. Download and install it?"
    select yn in "Yes" "No"; do
      case $yn in
        Yes ) break;;
        No ) exit;;
      esac
    done
    WD=$PWD
    TMPDIR=`mktemp -d`
    curl $URL-$ver.tar.gz | tar -xz --directory=$TMPDIR
    cd $TMPDIR/proj-$ver
    if (( `echo $ver | sed 's/\..*//'` < 8 )); then # pkgconfig supported with cmake since 8.0.0
      ./configure --prefix=$PREFIX/proj-$ver; make -j$(nproc); make install
    else
      mkdir build; cd build
      cmake -DCMAKE_INSTALL_PREFIX=$PREFIX/proj-$ver ..; make -j$(nproc); make install
    fi
    cd $WD
    rm -r $TMPDIR
  fi
  # compile and link
  if [ "$MODE" = "CMake" ]; then
    export PROJ_LIB="$PREFIX/proj-$ver/share/proj";
    cmake -DCMAKE_PREFIX_PATH="$PREFIX/proj-$ver" -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON -DENABLE_CCACHE=ON -GNinja $SRCPATH
    ninja utest
    ./utest
    rm -f CMakeCache.txt
  else
    export PROJ_MVER=`echo $ver | sed 's/\..*//'`
    export PROJ_LIBS=`pkg-config --libs --static $PREFIX/proj-$ver/lib/pkgconfig/proj.pc`
    export CXXJFLAGS="-DPROJ_VER=$PROJ_MVER -I"`pkg-config --variable=includedir $PREFIX/proj-$ver/lib/pkgconfig/proj.pc`
    export PROJ_LIB="$PREFIX/proj-$ver/share/proj"
    tclsh thcsdata.tcl $PROJ_LIB
    LD_LIBRARY_PATH=$PREFIX/proj-$ver/lib make -j$(nproc) tests
    rm thproj.o utest-proj.o thcsdata.o thcsdata.h
  fi
done

if [ "$MODE" = "CMake" ]; then
  cmake -DENABLE_CCACHE=ON -GNinja $SRCPATH
fi
