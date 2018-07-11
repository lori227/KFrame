

echo Starting Battle Cluster Server
start "Battle_Master" "KFStartupd.exe" "0" "50000" "./setting/master.startup"

echo Starting Battle Proxy Server
start "Battle_Proxy" "KFStartupd.exe" "0" "50001" "./setting/proxy.startup"

echo Starting Battle Shard Server
start "Battle_Shard" "KFStartupd.exe" "0" "52001" "./setting/shard.startup"


