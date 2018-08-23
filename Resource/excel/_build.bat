@echo off
setlocal enabledelayedexpansion
if not exist ../../Bin/config ( mkdir ../../Bin/config )

for /f "delims=\" %%x in ('dir /b /a-d /o-d "*.xlsx"') do (
echo %%x
_parse.exe %%~nx.xlsx --xml=../../Bin/config
)


