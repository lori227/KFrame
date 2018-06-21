@echo off

echo Starting Route Cluster Server
start "Route_Master" "KFStartupd.exe" "0" "3" "./setting/server.startup"

echo Starting Route Proxy Server
start "Route_Proxy" "KFStartupd.exe" "0" "3001" "./setting/proxy.startup"

echo Starting Route Shard Server
start "Route_Shard" "KFStartupd.exe" "0" "3101" "./setting/shard.startup"


