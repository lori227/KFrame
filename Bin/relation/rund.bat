@echo off

echo Starting Relation Cluster Server
start "Relation_Master" "KFStartupd.exe" "appid=100.0.71.1" "log=1" "startup=./setting/master.startup"

echo Starting Relation Proxy Server
start "Relation_Proxy" "KFStartupd.exe" "appid=100.0.72.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Relation Shard Server
start "Relation_Shard" "KFStartupd.exe" "appid=100.0.73.1" "log=1" "startup=./setting/shard.startup"


