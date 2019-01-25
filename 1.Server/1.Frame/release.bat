@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set releaseepath=..\9.Template
set localpath=.

if not exist %releaseepath% (mkdir %releaseepath% )
rem ===========================================================================

rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _bin path begin"
set binpath=_bin
if not exist %releaseepath%\%binpath% (mkdir %releaseepath%\%binpath% )

rem _gcm=================================
echo "release gcm begin"
set gcmpath=_bin\_gcm\builder
if not exist %releaseepath%\%gcmpath% (mkdir %releaseepath%\%gcmpath% )
copy /y %localpath%\%gcmpath%\gcm_build.exe %releaseepath%\%gcmpath%\
copy /y %localpath%\%gcmpath%\gcm_build %releaseepath%\%gcmpath%\

set gcmconfinput=_bin\_gcm\conf_input
if not exist %releaseepath%\%gcmconfinput% (mkdir %releaseepath%\%gcmconfinput% )
xcopy /y %localpath%\%gcmconfinput%\* %releaseepath%\%gcmconfinput%
echo "release gcm end"
rem ====================================

rem bin=================================
echo "release bin begin"
set binpath=_bin\bin
if not exist %releaseepath%\%binpath% (mkdir %releaseepath%\%binpath% )
xcopy /y %localpath%\%binpath%\*.dll %releaseepath%\%binpath%\
xcopy /y %localpath%\%binpath%\*.exe %releaseepath%\%binpath%\
echo "release bin end"
rem ====================================

rem setting=================================
echo "release setting begin"
set settingpath=_bin\setting
if not exist %releaseepath%\%settingpath% (mkdir %releaseepath%\%settingpath% )
xcopy /y %localpath%\%settingpath%\*.setting %releaseepath%\%settingpath%\
echo "release setting end"
rem ====================================

echo "release _bin path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _proto path begin"
set protopath=_proto
if not exist %releaseepath%\%protopath% (mkdir %releaseepath%\%protopath% )
copy /y %localpath%\%protopath%\1.win64.bat %releaseepath%\%protopath%\
copy /y %localpath%\%protopath%\2.linux.sh %releaseepath%\%protopath%\
copy /y %localpath%\%protopath%\3.protoc.exe %releaseepath%\%protopath%\
copy /y %localpath%\%protopath%\4.protoc %releaseepath%\%protopath%\
copy /y %localpath%\%protopath%\KFDefineMessage.proto %releaseepath%\%protopath%\
echo "release _proto path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _lib path begin"
set libpath=_lib

if not exist %releaseepath%\%libpath%\linux (mkdir %releaseepath%\%libpath%\linux )
xcopy /y %localpath%\%libpath%\linux\*.a %releaseepath%\%libpath%\linux
del %releaseepath%\%libpath%\linux\KFProtocol*.a

if not exist %releaseepath%\%libpath%\win64 (mkdir %releaseepath%\%libpath%\win64 )
xcopy /y %localpath%\%libpath%\win64\*.lib %releaseepath%\%libpath%\win64
del %releaseepath%\%libpath%\win64\KFProtocol*.lib
echo "release _lib path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
echo "release _build path begin"
set buildpath=_build

if not exist %releaseepath%\%buildpath%\linux (mkdir %releaseepath%\%buildpath%\linux )

if not exist %releaseepath%\%buildpath%\linux\tool (mkdir %releaseepath%\%buildpath%\linux\tool )
xcopy /y %localpath%\%buildpath%\linux\tool\* %releaseepath%\%buildpath%\linux\tool\
copy /y %localpath%\%buildpath%\linux\build_clean.sh %releaseepath%\%buildpath%\linux\
copy /y %localpath%\%buildpath%\linux\build_debug.sh %releaseepath%\%buildpath%\linux\
copy /y %localpath%\%buildpath%\linux\build_release.sh %releaseepath%\%buildpath%\linux\
copy /y %localpath%\%buildpath%\linux\make_develop.sh %releaseepath%\%buildpath%\linux\
copy /y %localpath%\%buildpath%\linux\make_online.sh %releaseepath%\%buildpath%\linux\
copy /y %localpath%\%buildpath%\linux\make_version.sh %releaseepath%\%buildpath%\linux\

echo "release _build path end"
rem ===========================================================================
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
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFGlobal.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFLogger
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFLogger.h %releaseepath%\%contribpath%\%subcontribpath%
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFSpdLog.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFMemory
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFMalloc.h %releaseepath%\%contribpath%\%subcontribpath%
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFMemoryDefine.h %releaseepath%\%contribpath%\%subcontribpath%

set subcontribpath=KFPlugin
if not exist %releaseepath%\%contribpath%\%subcontribpath% (mkdir %releaseepath%\%contribpath%\%subcontribpath% )
xcopy /y %localpath%\%contribpath%\%subcontribpath%\KFPlugin.h %releaseepath%\%contribpath%\%subcontribpath%



echo "release KFContrib path end"
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================


