@echo off

echo Starting Route Cluster Server
start "Route_Master" "KFStartupd.exe" "0" "30000" "./setting/master.startup"

echo Starting Route Proxy Server
start "Route_Proxy" "KFStartupd.exe" "0" "30001" "./setting/proxy.startup"

echo Starting Route Shard Server
start "Route_Shard" "KFStartupd.exe" "0" "32001" "./setting/shard.startup"


