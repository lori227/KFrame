@echo off
setlocal enabledelayedexpansion

set curpath=%cd% 
set svn=%cd%\%1\_subversion\bin\svn.exe

cd %cd%\%1
for /f "delims=" %%i in ('%svn% up ^| findstr "revision"') do set svnversion=%%i
set svnversion=%svnversion:~12%
echo %svnversion% > version.txt
cd %curpath%
