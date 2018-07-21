

echo Starting Log Cluster Server
start "Log_Master" "KFStartupd.exe" "0" "110000" "./setting/master.startup"

echo Starting Log Proxy Server
start "Log_Proxy" "KFStartupd.exe" "0" "110001" "./setting/proxy.startup"

echo Starting Log Shard Server
start "Log_Shard" "KFStartupd.exe" "0" "112001" "./setting/shard.startup"


