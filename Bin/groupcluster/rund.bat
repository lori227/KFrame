@echo off

echo Starting Group Cluster Server
start "Group_Master" "KFStartupd.exe" "0" "80000" "./setting/master.startup"

echo Starting Route Proxy Server
start "Group_Proxy" "KFStartupd.exe" "0" "80001" "./setting/proxy.startup"

echo Starting Route Shard Server
start "Group_Shard" "KFStartupd.exe" "0" "82001" "./setting/shard.startup"


