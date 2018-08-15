@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================


if not exist plugin (mkdir plugin)
if not exist setting (mkdir setting)
if not exist config (mkdir config)
if not exist script (mkdir script)

set path1=..\..\Server\_bin\win64
set path2=.

call :copyfile 1 KFStartupd.exe
call :copyfile 1 KFStartup.exe

set path1=..\..\Server\_lib\win64\3rd
set path2=.
call :copyfile 1 libeay32.dll
call :copyfile 1 ssleay32.dll

rem 全局配置
set path1=..\..\Server\_bin\setting
set path2=setting
call :copyfile 1 ip.address
call :copyfile 1 bus.relation
call :copyfile 1 server.network
call :copyfile 1 common.startup
call :copyfile 1 cluster.setting
call :copyfile 1 zone.setting
call :copyfile 1 kernel.setting

set path1=..\..\Server\_bin\setting\zone
set path2=setting

call :copyfile 1 gate.startup
call :copyfile 1 login.startup
call :copyfile 1 master.startup
call :copyfile 1 proxy.startup
call :copyfile 1 world.startup
call :copyfile 1 game.startup

rem 全局配置
set path1=..\..\Resource\config
set path2=config
xcopy /y /e %path1% %path2%\
rem 脚本
set path1=..\..\Resource\script
set path2=script
xcopy /y /e %path1% %path2%\

rem 插件目录
set path1=..\..\Server\_bin\win64\plugin
set path2=plugin

for /f "delims=" %%i in ('dir "%path1% *.dll" /b') do (
copy "%%i" "%path2%\" /y
)
xcopy /y %path1%\*.dll %path2%\


rem pause
rem ===========================================================================
rem ===========================================================================
rem ===========================================================================
rem 判断时间
:check_time
set need_copy=0

if not exist %file2% (
	set need_copy=1&exit/b:check_time
)

set file1_t=""
set file2_t=""

if %1==1 (
call:get_time file1
call:get_time file2 
)

if "%file1_t%" gtr "%file2_t%" (
	set need_copy=1
)
goto:eof

rem 获得时间
:get_time
for /f "skip=4 tokens=1,2" %%a in ('dir /tw "!!%~1!!"')do (
set %~1_t=%%a %%b&exit/b:get_time)
goto:eof

rem 拷贝文件
:copyfile 
set file1=%path1%\%2
set file2=%path2%\%2

call :check_time %1
if %need_copy%==1 (
echo %file1%
copy /y %file1% %file2%
) 
goto:eof
