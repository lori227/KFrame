@echo off

SETLOCAL ENABLEDELAYEDEXPANSION
cd 1.Server
call frame.bat
cd ..

cd 2.Client
call fream.bat
cd ..

cd 3.Resouce
call fream.bat
cd ..

rem ===========================================================================
