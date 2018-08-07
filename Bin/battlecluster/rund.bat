

echo Starting Battle Cluster Server
start "Battle_Master" "KFStartupd.exe" "appid=50000" "log=1" "startup=./setting/master.startup"

echo Starting Battle Proxy Server
start "Battle_Proxy" "KFStartupd.exe" "appid=50001" "log=1" "startup=./setting/proxy.startup"

echo Starting Battle Shard Server
start "Battle_Shard" "KFStartupd.exe" "appid=52001" "log=1" "startup=./setting/shard.startup"


