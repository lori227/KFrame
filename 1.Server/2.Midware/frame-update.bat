@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\9.Template
set localpath=.

mkdir %localpath%\_bin\setting1
xcopy /y /S %localpath%\_bin\setting\* %localpath%\_bin\setting1
mkdir %localpath%\_bin\_gcm\conf_input1
xcopy /y /S %localpath%\_bin\_gcm\conf_input\* %localpath%\_bin\_gcm\conf_input1

copy /y %localpath%\KFCommon\KFField.h %localpath%\KFCommon\KFField.h1

mkdir %localpath%\_excel1
del %localpath%\_excel\_parse.exe
xcopy /y /S %localpath%\_excel\* %localpath%\_excel1
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================

xcopy /y /S %framepath%\* %localpath%\
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================

xcopy /y /S %localpath%\_bin\setting1\* %localpath%\_bin\setting\
rd /Q /S %localpath%\_bin\setting1
xcopy /y /S %localpath%\_bin\_gcm\conf_input1\* %localpath%\_bin\_gcm\conf_input\
rd /Q /S %localpath%\_bin\_gcm\conf_input1

copy /y %localpath%\KFCommon\KFField.h1 %localpath%\KFCommon\KFField.h
del %localpath%\KFCommon\KFField.h1

xcopy /y /S %localpath%\_excel1\* %localpath%\_excel
rd /Q /S %localpath%\_excel1

rem ===========================================================================
