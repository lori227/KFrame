@echo off

echo Starting Public Cluster Server
start "Public_Master" "KFStartupd.exe" "0" "2" "./setting/master.startup"

echo Starting Public Proxy Server
start "Public_Proxy" "KFStartupd.exe" "0" "2001" "./setting/proxy.startup"

echo Starting Public Shard Server
start "Public_Shard" "KFStartupd.exe" "0" "2101" "./setting/shard.startup"


