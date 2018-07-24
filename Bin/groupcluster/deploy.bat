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

set path1=..\..\Source\_bin\win64
set path2=.

call :copyfile 1 KFStartupd.exe
call :copyfile 1 KFStartup.exe
call :copyfile 1 log4cxxd.dll
call :copyfile 1 log4cxx.dll
call :copyfile 1 libcrypto-1_1-x64.dll
call :copyfile 1 libssl-1_1-x64.dll

rem 全局配置
set path1=..\..\Source\_bin\setting
set path2=setting
call :copyfile 1 ip.address
call :copyfile 1 server.network
call :copyfile 1 bus.relation
call :copyfile 1 initapp.log4cxx
call :copyfile 1 templateapp.log4cxx
call :copyfile 1 common.startup
call :copyfile 1 cluster.setting

rem 配置目录
set path1=..\..\Source\_bin\setting\group
set path2=setting
call :copyfile 1 master.startup
call :copyfile 1 proxy.startup
call :copyfile 1 shard.startup

rem config
set path1=..\..\Resource\config
set path2=config
call :copyfile 1 kernel.config
call :copyfile 1 class.config
call :copyfile 1 group.config

rem 插件目录
set path1=..\..\Source\_bin\win64\plugin
set path2=plugin

call :copyfile 1 KFHttpClient.dll
call :copyfile 1 KFHttpClientd.dll
call :copyfile 1 KFConfigd.dll
call :copyfile 1 KFConfig.dll
call :copyfile 1 KFBusd.dll
call :copyfile 1 KFBus.dll
call :copyfile 1 KFIpAddressd.dll
call :copyfile 1 KFIpAddress.dll
call :copyfile 1 KFMessaged.dll
call :copyfile 1 KFMessage.dll
call :copyfile 1 KFTimerd.dll
call :copyfile 1 KFTimer.dll
call :copyfile 1 KFTcpServerd.dll
call :copyfile 1 KFTcpServer.dll
call :copyfile 1 KFTcpClientd.dll
call :copyfile 1 KFTcpClient.dll
call :copyfile 1 KFKerneld.dll
call :copyfile 1 KFKernel.dll
call :copyfile 1 KFDisplayd.dll
call :copyfile 1 KFDisplay.dll
call :copyfile 1 KFClusterProxy.dll
call :copyfile 1 KFClusterProxyd.dll
call :copyfile 1 KFClusterShardd.dll
call :copyfile 1 KFClusterShard.dll
call :copyfile 1 KFClusterServerd.dll
call :copyfile 1 KFClusterServer.dll
call :copyfile 1 KFGroupShardd.dll
call :copyfile 1 KFGroupShard.dll
call :copyfile 1 KFGroupProxyd.dll
call :copyfile 1 KFGroupProxy.dll
call :copyfile 1 KFDeployCommand.dll
call :copyfile 1 KFDeployCommandd.dll
call :copyfile 1 KFDeployClient.dll
call :copyfile 1 KFDeployClientd.dll
call :copyfile 1 KFClusterClient.dll
call :copyfile 1 KFClusterClientd.dll
call :copyfile 1 KFLogClient.dll
call :copyfile 1 KFLogClientd.dll


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
