@echo off
rem ʹ��32λ��64λ����
rem set Path=%Path:mingw64\bin=mingw32\bin%
mkdir build
cd .\build
cmake -G "MinGW Makefiles" .. > TriffleInfo
mingw32-make
pause