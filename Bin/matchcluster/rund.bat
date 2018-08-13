@echo off

echo Starting Match Cluster Server
start "Match_Master" "KFStartupd.exe" "appid=40000" "log=1" "startup=./setting/master.startup"

echo Starting Match Proxy Server
start "Match_Proxy" "KFStartupd.exe" "appid=40001" "log=1" "startup=./setting/proxy.startup"

echo Starting Match Shard Server
start "Match_Shard" "KFStartupd.exe" "appid=42001" "log=1" "startup=./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "appid=42002" "log=1" "startup=./setting/shard.startup"
start "Match_Shard" "KFStartupd.exe" "appid=42003" "log=1" "startup=./setting/shard.startup"
