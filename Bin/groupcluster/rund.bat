@echo off

echo Starting Group Cluster Server
start "Group_Master" "KFStartupd.exe" "appid=80000" "log=1" "startup=./setting/master.startup"

echo Starting Route Proxy Server
start "Group_Proxy" "KFStartupd.exe" "appid=80001" "log=1" "startup=./setting/proxy.startup"

echo Starting Route Shard Server
start "Group_Shard" "KFStartupd.exe" "appid=82001" "log=1" "startup=./setting/shard.startup"


