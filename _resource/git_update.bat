@echo on
setlocal enabledelayedexpansion

set curpath=%cd% 

cd %cd%\%1

git pull
git log -1 --pretty=format:%%h > version.txt

cd %curpath%

