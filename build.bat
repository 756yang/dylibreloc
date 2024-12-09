@echo off
rem 使用32位或64位编译
rem set Path=%Path:mingw64\bin=mingw32\bin%
mkdir build
cd .\build
cmake -G "MinGW Makefiles" .. > TriffleInfo
mingw32-make
pause