@echo off
setlocal enabledelayedexpansion

set curpath=%cd% 
set svn=%cd%\%1\_subversion\bin\svn.exe

cd %cd%\%1
%svn% commit -m "update resource by parse tool"
cd %curpath%




