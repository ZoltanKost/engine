compiled using mvsc using following commands: 

/engine:
mkdir builds
pushd builds
cl -Zi ..\src\engine.c ..\src\lib\raylibdll.lib /I ..\src\include
popd
