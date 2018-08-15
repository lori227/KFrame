@echo off

echo Starting Public Cluster Server
start "Public_Master" "KFStartupd.exe" "appid=100.0.21.1" "log=1" "startup=./setting/master.startup"

echo Starting Public Proxy Server
start "Public_Proxy" "KFStartupd.exe" "appid=100.0.22.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Public Shard Server
start "Public_Shard" "KFStartupd.exe" "appid=100.0.23.1" "log=1" "startup=./setting/shard.startup"


