@echo off
setlocal enabledelayedexpansion

set curpath=%cd% 

set projectname=%2
set svnversion=%3
set version=%4
set filename=%5
set serverurl=%6

::pack
cd %cd%\%1\_gcm\builder\
tar -zcvf %filename% ..\conf_output\*

certutil -hashfile %filename% MD5 >APP_MD5_TXT
set index=0
for /f "tokens=* delims= " %%i in (APP_MD5_TXT) do (
	if !index! == 1 (set md5=%%i)
 	set /a index=index+1
)
del APP_MD5_TXT

:: upload 
gcm_build.exe -p %projectname% -s %svnversion% -b "develop" -c 1.1 -m "debug" -v %version% -n 1.2 -t 3 -fn %filename% -md5 %md5%
del %filename%

::reload
curl -d "{\"command\":\"resource\", \"value\":\"%filename%\", \"appname\":\"*\", \"apptype\":\"*\", \"appid\":\"*\", \"zoneid\":\"0\"}" %serverurl%

:: 切换回当前目录
cd %curpath%