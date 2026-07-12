@echo off
rem Build the Slabb library through Ninja, after sourcing the MSVC environment.
rem Equivalent to opening "x64 Native Tools Command Prompt for VS 2022" and running
rem `cmake --build out\build\x64-Debug --target Slabb` from there.

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo Failed to set up MSVC environment.
    exit /b 1
)

cd /d "D:\Tin\Private\Projects\C++\Slabb"
cmake --build out\build\x64-Debug --target Slabb %*
