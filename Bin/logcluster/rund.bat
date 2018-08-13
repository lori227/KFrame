

echo Starting Log Cluster Server
start "Log_Master" "KFStartupd.exe" "appid=110000" "log=1" "startup=./setting/master.startup"

echo Starting Log Proxy Server
start "Log_Proxy" "KFStartupd.exe" "appid=110001" "log=1" "startup=./setting/proxy.startup"

echo Starting Log Shard Server
start "Log_Shard" "KFStartupd.exe" "appid=112001" "log=1" "startup=./setting/shard.startup"


