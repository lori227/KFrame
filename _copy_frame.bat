@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=D:\Work\Frame\trunk
set localpath=D:\KFrame\KFrame
rem ===========================================================================
rem ===========================================================================

echo "update _bin path begin"
set subpath=_bin
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

set subpath=_lib
rd /s /q %localpath%\%subpath%
if not exist %localpath%\%subpath% ( mkdir %localpath%\%subpath% )
xcopy /y /S %framepath%\%subpath%\* %localpath%\%subpath%\

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

copy /y %framepath%\.gitignore  %localpath%\
copy /y %framepath%\CMakeLists.txt  %localpath%\
copy /y %framepath%\Frame.sln  %localpath%\
copy /y %framepath%\LICENSE  %localpath%\
copy /y %framepath%\README.md  %localpath%\