

echo Starting Mail Cluster Server
start "Mail_Master" "KFStartupd.exe" "0" "6" "./setting/master.startup"

echo Starting Mail Proxy Server
start "Mail_Proxy" "KFStartupd.exe" "0" "6001" "./setting/proxy.startup"

echo Starting Mail Shard Server
start "Mail_Shard" "KFStartupd.exe" "0" "6101" "./setting/shard.startup"


