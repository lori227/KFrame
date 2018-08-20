@echo off

echo Starting Guild Cluster Server
start "guild-master" "KFStartupd.exe" "appid=100.0.91.1" "log=1" "startup=./setting/master.startup"

echo Starting Guild Proxy Server
start "guild-proxy" "KFStartupd.exe" "appid=100.0.92.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Guild Shard Server
start "guild-shard" "KFStartupd.exe" "appid=100.0.93.1" "log=1" "startup=./setting/shard.startup"


