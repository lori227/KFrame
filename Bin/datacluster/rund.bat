@echo off

echo Starting Data Cluster Server
start "Data_Master" "KFStartupd.exe" "appid=10000" "log=1" "startup=./setting/master.startup"

echo Starting Data Proxy Server
start "Data_Proxy" "KFStartupd.exe" "appid=10001" "log=1" "startup=./setting/proxy.startup"

echo Starting Data Shard Server
start "Data_Shard" "KFStartupd.exe" "appid=12006" "log=1" "startup=./setting/shard.startup"


