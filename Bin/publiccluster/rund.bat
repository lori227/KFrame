@echo off

echo Starting Public Cluster Server
start "Public_Master" "KFStartupd.exe" "0" "20000" "./setting/master.startup"

echo Starting Public Proxy Server
start "Public_Proxy" "KFStartupd.exe" "0" "20001" "./setting/proxy.startup"

echo Starting Public Shard Server
start "Public_Shard" "KFStartupd.exe" "0" "22001" "./setting/shard.startup"


