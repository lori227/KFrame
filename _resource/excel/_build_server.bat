@echo off
setlocal enabledelayedexpansion
if not exist ..\config ( mkdir ..\config )
if not exist ..\config\server ( mkdir ..\config\server )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_zparse.exe %%~nx.xlsx 1 --xml=..\config\server
)