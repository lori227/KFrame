@echo off

echo Starting Relation Cluster Server
start "Relation_Master" "KFStartupd.exe" "appid=70000" "log=1" "startup=./setting/master.startup"

echo Starting Relation Proxy Server
start "Relation_Proxy" "KFStartupd.exe" "appid=70001" "log=1" "startup=./setting/proxy.startup"

echo Starting Relation Shard Server
start "Relation_Shard" "KFStartupd.exe" "appid=72001" "log=1" "startup=./setting/shard.startup"


