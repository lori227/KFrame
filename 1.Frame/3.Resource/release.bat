@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set releasepath=..\9.Template\3.Resource
set localpath=.

rd /Q /S %releasepath%
if not exist %releasepath% (mkdir %releasepath% )
rem ===========================================================================

rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release excel path begin"
set excelpath=excel
if not exist %releasepath%\%excelpath% (mkdir %releasepath%\%excelpath% )

xcopy /y /S %localpath%\%excelpath% %releasepath%\%excelpath%\
del %releasepath%\%excelpath%\_parsed.exe

echo "release path path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release proto path begin"
set protopath=proto
if not exist %releasepath%\%protopath% (mkdir %releasepath%\%protopath% )

xcopy /y /S %localpath%\%protopath% %releasepath%\%protopath%\
del %releasepath%\%protopath%\KFFrameMessage.proto

echo "release proto path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release tool path begin"
set toolpath=tool
if not exist %releasepath%\%toolpath% (mkdir %releasepath%\%toolpath% )
xcopy /y /S %localpath%\%toolpath% %releasepath%\%toolpath%\
echo "release tool path end"