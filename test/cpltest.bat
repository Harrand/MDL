@echo off
set scriptdir=%cd%
cd..
cd src
set srcdir=%cd%
cd..
cd cpl
set cpldir=%cd%
cd %scriptdir%
g++ -c -std=c++11 test.cpp -I %srcdir%
g++ -o test.exe test.o -L%cpldir% -lmdl
pause