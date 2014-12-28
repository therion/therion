#!/bin/sh
swig -python -c++ geomag.i
g++ -shared -fPIC ../../thgeomag.cxx geomag_wrap.cxx -I /usr/include/python2.7 -o _geomag.so
rm *wrap*
./test.py