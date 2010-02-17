#!/bin/sh
swig -python -c++ geomag.i
g++ -shared ../../thgeomag.cxx geomag_wrap.cxx -I /usr/include/python2.5 -o _geomag.so
rm *wrap*
./test.py