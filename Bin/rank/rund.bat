@echo off

echo Starting Cluster Server
start "rank-master" "KFStartupd.exe" "appid=100.0.91.1" "log=1" "startup=./setting/master.startup"

echo Starting  Proxy Server
start "rank-proxy" "KFStartupd.exe" "appid=100.0.92.1" "log=1" "startup=./setting/proxy.startup"

echo Starting  Shard Server
start "rank-shard" "KFStartupd.exe" "appid=100.0.93.1" "log=1" "startup=./setting/shard.startup"


