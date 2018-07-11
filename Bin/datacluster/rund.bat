@echo off

echo Starting Data Cluster Server
start "Data_Master" "KFStartupd.exe" "0" "10000" "./setting/master.startup"

echo Starting Data Proxy Server
start "Data_Proxy" "KFStartupd.exe" "0" "10001" "./setting/proxy.startup"

echo Starting Data Shard Server
start "Data_Shard" "KFStartupd.exe" "0" "12006" "./setting/shard.startup"


