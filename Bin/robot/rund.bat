@echo off
echo Starting Robot Server
start "Robot" "KFStartupd.exe" "appid=1" "log=1" "startup=./setting/robot.startup"
