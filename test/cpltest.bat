@echo off
set scriptdir=%cd%
cd..
cd src
set srcdir=%cd%
cd..
cd cpl
set cpldir=%cd%
cd %scriptdir%
g++ -c -std=c++14 -Wall -pedantic-errors -O3 test.cpp -I %srcdir%
g++ -o test.exe test.o -L. -lmdl
pause