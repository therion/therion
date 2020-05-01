#!/bin/sh
swig -python -c++ geomag.i
g++ -shared -fPIC ../../thgeomag.cxx geomag_wrap.cxx `python3-config --includes` -o _geomag.so
rm *wrap*
./test.py
rm _geomag.so geomag.py