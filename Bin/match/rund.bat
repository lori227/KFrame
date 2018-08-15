@echo off

echo Starting Match Cluster Server
start "Match_Master" "KFStartupd.exe" "appid=100.0.41.1" "log=1" "startup=./setting/master.startup"

echo Starting Match Proxy Server
start "Match_Proxy" "KFStartupd.exe" "appid=100.0.42.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Match Shard Server
start "Match_Shard" "KFStartupd.exe" "appid=100.0.43.1" "log=1" "startup=./setting/shard.startup"
