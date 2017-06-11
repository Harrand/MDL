@echo off

set cpldir=%cd%\cpl
echo Compiling MDL Source
cd src
g++ -c -std=c++14 -Wall -pedantic-errors -O3 -DMDLLBUILD mdl.cpp
echo Building MDL Dynamic Link Library (via mdl.o)

if not exist "%cpldir%" mkdir "%cpldir%"
for %%o in (.o) do move "*%%o" "%cpldir%"
cd "%cpldir%"

g++ -shared -o mdl.dll mdl.o -Wl,-no-undefined,--enable-runtime-pseudo-reloc,--out-implib,libmdl.a
echo MDL Dynamic Link Library build attempted (output is mdl.dll and libmdl.a). Ensure that the process was successful!
pause