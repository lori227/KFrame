@echo off

rem build 32-bit
rem call "%~p0%build.generic.cmd" VS2017Enterprise Win32 Release v141

rem build 64-bit
call "%~p0%build.generic.cmd" VS2017Enterprise x64 Debug v141
call "%~p0%build.generic.cmd" VS2017Enterprise x64 Release v141