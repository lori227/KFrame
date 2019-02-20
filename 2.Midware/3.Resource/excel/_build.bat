@echo off
setlocal enabledelayedexpansion
if not exist ..\config ( mkdir ..\config )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_parse.exe %%~nx.xlsx --xml=..\config
)


if not exist ..\..\1.Server\_bin\config\ ( mkdir ..\..\1.Server\_bin\config\ )
xcopy /y ..\config\* ..\..\1.Server\_bin\config\