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

set path1=..\..\Server\_bin\win64
set path2=.

call :copyfile 1 KFStartupd.exe
call :copyfile 1 KFStartup.exe

set path1=..\..\Server\_lib\win64\3rd
set path2=.
call :copyfile 1 libeay32.dll
call :copyfile 1 ssleay32.dll
call :copyfile 1 libmysql.dll

rem 全局配置
set path1=..\..\Server\_bin\setting
set path2=setting
call :copyfile 1 redis.address
call :copyfile 1 common.startup

rem 配置目录
set path1=..\..\Server\_bin\setting\guild
call :copyfile 1 master.startup
call :copyfile 1 proxy.startup
call :copyfile 1 shard.startup

rem config
set path1=..\..\Resource\config
set path2=config
call :copyfile 1 option.config
call :copyfile 1 kernel.config
call :copyfile 1 class.config

rem 插件目录
set path1=..\..\Server\_bin\win64\plugin
set path2=plugin

call :copyfile 1 KFHttpClient.dll
call :copyfile 1 KFHttpClientd.dll
call :copyfile 1 KFConfigd.dll
call :copyfile 1 KFConfig.dll
call :copyfile 1 KFOptiond.dll
call :copyfile 1 KFOption.dll
call :copyfile 1 KFConnectiond.dll
call :copyfile 1 KFConnection.dll
call :copyfile 1 KFMessaged.dll
call :copyfile 1 KFMessage.dll
call :copyfile 1 KFTimerd.dll
call :copyfile 1 KFTimer.dll
call :copyfile 1 KFTcpServerd.dll
call :copyfile 1 KFTcpServer.dll
call :copyfile 1 KFTcpClientd.dll
call :copyfile 1 KFTcpClient.dll
call :copyfile 1 KFRedisd.dll
call :copyfile 1 KFRedis.dll
call :copyfile 1 KFWorkerd.dll
call :copyfile 1 KFWorker.dll
call :copyfile 1 KFClusterProxyd.dll
call :copyfile 1 KFClusterProxy.dll
call :copyfile 1 KFClusterShardd.dll
call :copyfile 1 KFClusterShard.dll
call :copyfile 1 KFClusterMasterd.dll
call :copyfile 1 KFClusterMaster.dll
call :copyfile 1 KFGuildShardd.dll
call :copyfile 1 KFGuildShard.dll
call :copyfile 1 KFGuildProxyd.dll
call :copyfile 1 KFGuildProxy.dll
call :copyfile 1 KFScheduled.dll
call :copyfile 1 KFSchedule.dll
call :copyfile 1 KFKerneld.dll
call :copyfile 1 KFKernel.dll
call :copyfile 1 KFDeployCommand.dll
call :copyfile 1 KFDeployCommandd.dll
call :copyfile 1 KFDeployClient.dll
call :copyfile 1 KFDeployClientd.dll


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
