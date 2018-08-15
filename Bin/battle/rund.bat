

echo Starting Battle Cluster Server
start "Battle_Master" "KFStartupd.exe" "appid=100.0.51.1" "log=1" "startup=./setting/master.startup"

echo Starting Battle Proxy Server
start "Battle_Proxy" "KFStartupd.exe" "appid=100.0.52.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Battle Shard Server
start "Battle_Shard" "KFStartupd.exe" "appid=100.0.53.1" "log=1" "startup=./setting/shard.startup"


