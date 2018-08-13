@echo off

echo Starting Public Cluster Server
start "Public_Master" "KFStartupd.exe" "appid=20000" "log=1" "startup=./setting/master.startup"

echo Starting Public Proxy Server
start "Public_Proxy" "KFStartupd.exe" "appid=20001" "log=1" "startup=./setting/proxy.startup"

echo Starting Public Shard Server
start "Public_Shard" "KFStartupd.exe" "appid=22001" "log=1" "startup=./setting/shard.startup"


