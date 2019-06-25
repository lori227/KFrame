@echo off
setlocal enabledelayedexpansion
if not exist ..\config ( mkdir ..\config )
if not exist ..\config\client ( mkdir ..\config\client )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_parse.exe %%~nx.xlsx 2 --xml=..\config\client
)