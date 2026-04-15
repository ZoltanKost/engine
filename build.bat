@echo off
cd w:/Engine/
mkdir builds
pushd builds
cl -Zi ..\src\*.c ..\src\lib\raylibdll.lib /I ..\src\include
popd

