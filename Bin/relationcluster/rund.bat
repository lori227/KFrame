@echo off

echo Starting Relation Cluster Server
start "Relation_Master" "KFStartupd.exe" "0" "7" "./setting/master.startup"

echo Starting Relation Proxy Server
start "Relation_Proxy" "KFStartupd.exe" "0" "7001" "./setting/proxy.startup"

echo Starting Relation Shard Server
start "Relation_Shard" "KFStartupd.exe" "0" "7101" "./setting/shard.startup"


