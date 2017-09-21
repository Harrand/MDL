@echo off

set cpldir=%cd%\cpl
echo Compiling MDL Source
cd src
g++ -c -std=c++17 -Wall -Wextra -pedantic-errors -O3 *.cpp
echo Building MDL Dynamic Link Library (via mdl.o)

if not exist "%cpldir%" mkdir "%cpldir%"
for %%o in (.o) do move "*%%o" "%cpldir%"
cd "%cpldir%"

g++ -std=c++17 -shared -o mdl.dll *.o -Wl,-no-undefined,--enable-runtime-pseudo-reloc,--out-implib,libmdl.a
echo MDL Dynamic Link Library build attempted (output is mdl.dll and libmdl.a). Ensure that the process was successful!
pause