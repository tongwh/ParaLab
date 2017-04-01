@SETLOCAL
@SET CMAKE_EXEC="D:\Program Files (x86)\CMake 2.8\bin\cmake.exe"
@IF EXIST %CMAKE_EXEC% GOTO START
@GOTO ERROR

:START
mkdir build
cd build
%CMAKE_EXEC% -G "Visual Studio 12 Win64" ..

cd ..
@GOTO EOF


@:ERROR
@echo ERROR: CMake not found.

:EOF