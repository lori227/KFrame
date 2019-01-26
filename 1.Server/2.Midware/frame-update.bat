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

xcopy /y /S %framepath%\* %localpath%\

xcopy /y /S %localpath%\_bin\setting1\* %localpath%\_bin\setting\
rd /Q /S %localpath%\_bin\setting1
xcopy /y /S %localpath%\_bin\_gcm\conf_input1\* %localpath%\_bin\_gcm\conf_input\
rd /Q /S %localpath%\_bin\_gcm\conf_input1

ren %localpath%\KFCommon\KFField.h1 KFField.h
rem ===========================================================================
