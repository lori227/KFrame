@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set framepath=..\1.Frame
set localpath=.

if not exist %framepath% (mkdir %framepath% )
rem ===========================================================================

rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _bin path begin"
set binpath=_bin
if not exist %localpath%\%binpath% (mkdir %localpath%\%binpath% )
xcopy /y /S %framepath%\%binpath%\gcm_build.exe %localpath%\%binpath%\
echo "release _bin path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _proto path begin"
set protopath=_proto
if not exist %localpath%\%protopath% (mkdir %localpath%\%protopath% )
xcopy /y /S %framepath%\%protopath%\ %localpath%\%protopath%\
echo "release _proto path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _lib path begin"
set libpath=_lib

if not exist %localpath%\%libpath% (mkdir %localpath%\%libpath% )
xcopy /y /S %framepath%\%libpath%\ %localpath%\%libpath%\
echo "release _lib path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _build path begin"
set buildpath=_build
if not exist %localpath%\%buildpath%\ (mkdir %localpath%\%buildpath% )
xcopy /y /S %framepath%\%buildpath%\ %localpath%\%buildpath%\

echo "release _build path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _tool path begin"
set toolpath=_tool
if not exist %releaseepath%\%toolpath% (mkdir %releaseepath%\%toolpath% )
xcopy /y %localpath%\%toolpath%\* %releaseepath%\%toolpath%\
echo "release _tool path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================

echo "release KFCommon path begin"
set commonpath=KFCommon
if not exist %releaseepath%\%commonpath% (mkdir %releaseepath%\%commonpath% )
xcopy /y %localpath%\%commonpath%\* %releaseepath%\%commonpath%\

echo "release KFCommon path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release KFContrib path begin"
set contribpath=KFContrib
if not exist %releaseepath%\%contribpath% (mkdir %releaseepath%\%contribpath% )

set subcontribpath=KFConHash
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\*.h %releaseepath%\%contribpath%\%subcontribpath%
xcopy /y %localpath%\%contribpath%\%subcontribpath%\*.inl %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFCore
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
copy /y %localpath%\%contribpath%\%subcontribpath%\KFData.h %releaseepath%\%contribpath%\%subcontribpath%\
copy /y %localpath%\%contribpath%\%subcontribpath%\KFData.inl %releaseepath%\%contribpath%\%subcontribpath%\
copy /y %localpath%\%contribpath%\%subcontribpath%\KFDataSetting.h %releaseepath%\%contribpath%\%subcontribpath%\

set subcontribpath=KFGlobal
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
copy /y %localpath%\%contribpath%\%subcontribpath%\KFGlobal.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFLogger
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
copy /y %localpath%\%contribpath%\%subcontribpath%\KFLogger.h %releaseepath%\%contribpath%\%subcontribpath%
copy /y %localpath%\%contribpath%\%subcontribpath%\KFSpdLog.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFMemory
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
copy /y %localpath%\%contribpath%\%subcontribpath%\KFMalloc.h %releaseepath%\%contribpath%\%subcontribpath%
copy /y %localpath%\%contribpath%\%subcontribpath%\KFMemoryDefine.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFPlugin
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\*.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFProto
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
copy /y %localpath%\%contribpath%\%subcontribpath%\KFProto.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFUtility
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\*.h %releaseepath%\%contribpath%\%subcontribpath%
xcopy /y %localpath%\%contribpath%\%subcontribpath%\*.inl %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFProtocol
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\* %releaseepath%\%contribpath%\%subcontribpath%

echo "release KFContrib path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release KFLibrary path begin"
set librarypath=KFLibrary
if not exist %releaseepath%\%librarypath% (mkdir %releaseepath%\%librarypath% )

set sublibrarypath=lua\lua53
if not exist %releaseepath%\%librarypath%\%sublibrarypath% (mkdir %releaseepath%\%librarypath%\%sublibrarypath% )
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\*.h %releaseepath%\%librarypath%\%sublibrarypath%
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\*.inl %releaseepath%\%librarypath%\%sublibrarypath%

set sublibrarypath=luaplus
if not exist %releaseepath%\%librarypath%\%sublibrarypath% (mkdir %releaseepath%\%librarypath%\%sublibrarypath% )
if not exist %releaseepath%\%librarypath%\%sublibrarypath%\tilde (mkdir %releaseepath%\%librarypath%\%sublibrarypath%\tilde )
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\*.h %releaseepath%\%librarypath%\%sublibrarypath%
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\*.inl %releaseepath%\%librarypath%\%sublibrarypath%
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\tilde\*.h %releaseepath%\%librarypath%\%sublibrarypath%\tilde
xcopy /y %localpath%\%librarypath%\%sublibrarypath%\tilde\*.inl %releaseepath%\%librarypath%\%sublibrarypath%\tilde

set sublibrarypath=rapidjson
if not exist %releaseepath%\%librarypath%\%sublibrarypath% (mkdir %releaseepath%\%librarypath%\%sublibrarypath% )
xcopy /y /S %localpath%\%librarypath%\%sublibrarypath%\* %releaseepath%\%librarypath%\%sublibrarypath%

set sublibrarypath=google
if not exist %releaseepath%\%librarypath%\%sublibrarypath% (mkdir %releaseepath%\%librarypath%\%sublibrarypath% )
xcopy /y /S %localpath%\%librarypath%\%sublibrarypath%\protobuf\src\google\* %releaseepath%\%librarypath%\%sublibrarypath%

echo "release KFLibrary path end"
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
