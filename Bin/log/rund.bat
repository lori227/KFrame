

echo Starting Log Cluster Server
start "Log_Master" "KFStartupd.exe" "appid=100.0.111.1" "log=1" "startup=./setting/master.startup"

echo Starting Log Proxy Server
start "Log_Proxy" "KFStartupd.exe" "appid=100.0.112.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Log Shard Server
start "Log_Shard" "KFStartupd.exe" "appid=100.0.113.1" "log=1" "startup=./setting/shard.startup"


