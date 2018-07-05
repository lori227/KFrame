@echo off

echo Starting Group Cluster Server
start "Group_Master" "KFStartupd.exe" "0" "8" "./setting/master.startup"

echo Starting Route Proxy Server
start "Group_Proxy" "KFStartupd.exe" "0" "8001" "./setting/proxy.startup"

echo Starting Route Shard Server
start "Group_Shard" "KFStartupd.exe" "0" "8101" "./setting/shard.startup"


