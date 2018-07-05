@echo off

echo Starting Match Cluster Server
start "Match_Master" "KFStartupd.exe" "0" "4" "./setting/master.startup"

echo Starting Match Proxy Server
start "Match_Proxy" "KFStartupd.exe" "0" "4001" "./setting/proxy.startup"

echo Starting Match Shard Server
start "Match_Shard" "KFStartupd.exe" "0" "4101" "./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "0" "4102" "./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "0" "4103" "./setting/shard.startup"
