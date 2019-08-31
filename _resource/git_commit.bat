@echo off
setlocal enabledelayedexpansion

set curpath=%cd% 

cd %cd%\%1
::git add *
git commit -m "update resource by parse tool" *
git push

cd %curpath%




