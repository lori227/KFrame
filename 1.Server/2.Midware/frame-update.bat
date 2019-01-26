@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\9.Template
set localpath=.
xcopy /y /S %framepath%\* %localpath%\
rem ===========================================================================
