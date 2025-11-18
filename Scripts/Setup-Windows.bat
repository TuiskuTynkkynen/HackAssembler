@echo off

pushd ..

if not exist .\Vendor\premake\bin\release\ (
    pushd .\Vendor\premake\
    cmd /C .\Bootstrap.bat
    popd
)

.\Vendor\premake\bin\release\premake5.exe --file=Build.lua vs2022

popd
pause