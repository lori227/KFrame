@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=D:\Work\Frame\trunk
set localpath=D:\KFrame
rem ===========================================================================
rem ===========================================================================

echo "update _bin path begin"
set subpath=_bin
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\_gcm\* %localpath%\%subpath%\_gcm\
xcopy /y /S %framepath%\%subpath%\config\* %localpath%\%subpath%\config\
xcopy /y /S %framepath%\%subpath%\script\* %localpath%\%subpath%\script\
xcopy /y /S %framepath%\%subpath%\setting\* %localpath%\%subpath%\setting\
xcopy /y /S %framepath%\%subpath%\startup\* %localpath%\%subpath%\startup\
mkdir %localpath%\%subpath%\bin\win64\debug
copy /y %framepath%\%subpath%\bin\win64\debug\libmysql.dll  %localpath%\%subpath%\bin\win64\debug\
copy /y %framepath%\%subpath%\bin\win64\debug\libtcmalloc_minimal.dll  %localpath%\%subpath%\bin\win64\debug\

mkdir %localpath%\%subpath%\bin\win64\release
copy /y %framepath%\%subpath%\bin\win64\release\libmysql.dll  %localpath%\%subpath%\bin\win64\release\
copy /y %framepath%\%subpath%\bin\win64\release\libtcmalloc_minimal.dll  %localpath%\%subpath%\bin\win64\release\

set subpath=_build
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=_doc
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=_resource
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=KFCommon
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=KFContrib
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=KFPlugin
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=KFStartup
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=KFTools
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

copy /y %framepath%\%subpath%\.gitignore  %localpath%\%subpath%\
copy /y %framepath%\%subpath%\_update_frame.bat  %localpath%\%subpath%\
copy /y %framepath%\%subpath%\CMakeLists.txt  %localpath%\%subpath%\
copy /y %framepath%\%subpath%\Frame.sln  %localpath%\%subpath%\
copy /y %framepath%\%subpath%\LICENSE  %localpath%\%subpath%\
copy /y %framepath%\%subpath%\README.md  %localpath%\%subpath%\