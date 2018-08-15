@echo off

echo Starting Route Cluster Server
start "Route_Master" "KFStartupd.exe" "appid=100.0.31.1" "log=1" "startup=./setting/master.startup"

echo Starting Route Proxy Server
start "Route_Proxy" "KFStartupd.exe" "appid=100.0.32.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Route Shard Server
start "Route_Shard" "KFStartupd.exe" "appid=100.0.33.1" "log=1" "startup=./setting/shard.startup"


