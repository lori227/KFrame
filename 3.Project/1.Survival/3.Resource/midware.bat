@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\..\..\2.Midware\3.Resource
set localpath=.

xcopy /y /S %framepath%\tool %localpath%\tool\

if not exist %localpath%\proto (
    xcopy /y /S %framepath%\proto %localpath%\proto\
)

rem ===========================================================================
rem ===========================================================================
mkdir %localpath%\excel1
xcopy /y /S %localpath%\excel\* %localpath%\excel1
del %localpath%\excel1\_parse.exe

xcopy /y /S %framepath%\excel %localpath%\excel\

xcopy /y /S %localpath%\excel1\* %localpath%\excel
rd /Q /S %localpath%\excel1
rem ===========================================================================
rem ===========================================================================

