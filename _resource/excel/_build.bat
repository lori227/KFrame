@echo off
setlocal enabledelayedexpansion
if not exist ..\..\_bin\config ( mkdir ..\..\_bin\config )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_parse.exe %%~nx.xlsx --xml=..\..\_bin\config
)