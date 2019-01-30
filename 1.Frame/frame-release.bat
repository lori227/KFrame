@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
rem 1.Server
cd 1.Server
call release.bat
cd ../

rem 2.Client
cd 2.Client
call release.bat
cd ../

rem 3.Resource
cd 3.Resource
call release.bat
cd ../

