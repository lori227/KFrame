@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\..\..\2.Midware\1.Server
set localpath=.

rem ===========================================================================
if not exist %localpath%\_bin\startup (mkdir %localpath%\_bin\startup )
xcopy /y %framepath%\_bin\startup\*.startup %localpath%\_bin\startup\
rem ===========================================================================
