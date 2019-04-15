@echo off
setlocal enabledelayedexpansion
if not exist ..\config ( mkdir ..\config )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_parse.exe %%~nx.xlsx --xml=..\config
)