@echo off

SETLOCAL ENABLEDELAYEDEXPANSION
set releaseepath=..\9.Template
set localpath=.

if not exist %releaseepath% (mkdir %releaseepath% )
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release KFPlugin path begin"
set pluginpath=KFInterface
if not exist %releaseepath%\%pluginpath% (mkdir %releaseepath%\%pluginpath% )

cd KFPlugin
for /d %%i in (*) do (
	copy /y %%i\*Interface.h ..\%releaseepath%\%pluginpath%\
)
cd ..\
echo "release KFPlugin path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
