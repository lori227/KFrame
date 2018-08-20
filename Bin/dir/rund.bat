

echo Starting Dir Cluster Server
start "Dir_Master" "KFStartupd.exe" "appid=100.0.121.1" "log=1" "startup=./setting/master.startup"

echo Starting Mail Proxy Server
start "Dir_Proxy" "KFStartupd.exe" "appid=100.0.122.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Mail Shard Server
start "Dir_Shard" "KFStartupd.exe" "appid=100.0.123.1" "log=1" "startup=./setting/shard.startup"


