@echo off

echo Starting Group Cluster Server
start "Group_Master" "KFStartupd.exe" "appid=100.0.81.1" "log=1" "startup=./setting/master.startup"

echo Starting Route Proxy Server
start "Group_Proxy" "KFStartupd.exe" "appid=100.0.82.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Route Shard Server
start "Group_Shard" "KFStartupd.exe" "appid=100.0.83.1" "log=1" "startup=./setting/shard.startup"


