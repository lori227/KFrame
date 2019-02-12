@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
cd 1.Server
call frame.bat
call midware.bat
cd ..
 
cd 2.Client
call frame.bat
call midware.bat
cd ..

cd 3.Resource
call frame.bat
call midware.bat
cd ..