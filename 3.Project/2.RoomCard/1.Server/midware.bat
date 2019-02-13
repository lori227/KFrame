@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\..\..\2.Midware\1.Server
set localpath=.

rem ===========================================================================
if not exist %localpath%\_bin\startup (mkdir %localpath%\_bin\startup )
xcopy /y %framepath%\_bin\startup\*.startup %localpath%\_bin\startup\
rem ===========================================================================
if not exist %localpath%\KFContrib\KFProtocol\KFProtocol.vcxproj (
    copy /y %framepath%\KFContrib\KFProtocol\CMakeLists.txt %localpath%\KFContrib\KFProtocol\
    copy /y %framepath%\KFContrib\KFProtocol\KFProtocol.h %localpath%\KFContrib\KFProtocol\
    copy /y %framepath%\KFContrib\KFProtocol\KFProtocol.vcxproj %localpath%\KFContrib\KFProtocol\
    copy /y %framepath%\KFContrib\KFProtocol\KFProtocol.vcxproj.filters %localpath%\KFContrib\KFProtocol\
     )
rem ===========================================================================
copy /y %localpath%\KFCommon\KFField.h %localpath%\KFCommon\KFField.h1
copy /y %framepath%\KFCommon\KFField.h %localpath%\KFCommon\
copy /y %localpath%\KFCommon\KFField.h1 %localpath%\KFCommon\KFField.h
del %localpath%\KFCommon\KFField.h1
rem ===========================================================================

if not exist %localpath%\KFPlugin (
    mkdir %localpath%\KFPlugin
    copy /y %framepath%\Midware.sln %localpath%\RoomCard.sln
    copy /y %framepath%\CMakeLists.txt %localpath%\
 )

cd %framepath%\KFPlugin
set pluginpath=..\..\..\3.Project\1.Survival\1.Server\KFPlugin

if not exist %pluginpath%\%%i ( xcopy /y /S %%i %pluginpath%\%%i\ )
rem ===========================================================================

