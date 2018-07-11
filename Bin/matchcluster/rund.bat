@echo off

echo Starting Match Cluster Server
start "Match_Master" "KFStartupd.exe" "0" "40000" "./setting/master.startup"

echo Starting Match Proxy Server
start "Match_Proxy" "KFStartupd.exe" "0" "40001" "./setting/proxy.startup"

echo Starting Match Shard Server
start "Match_Shard" "KFStartupd.exe" "0" "42001" "./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "0" "42002" "./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "0" "42003" "./setting/shard.startup"
