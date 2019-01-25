@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\1.Frame
set localpath=.
rem ===========================================================================


if not exist groupcluster (mkdir groupcluster)
xcopy /y %path1%\matchcluster\*.bat %path2%\matchcluster