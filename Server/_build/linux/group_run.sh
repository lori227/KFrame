
basepath=$(cd `dirname $0`; pwd)
cd /data/groupcluster
server=groupserver

if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Cluster Server
./$server 0 8 ./setting/server.startup

echo Starting Proxy Server
./$server 0 8001 ./setting/proxy.startup
./$server 0 8002 ./setting/proxy.startup

echo Starting Shard Server
./$server 0 8101 ./setting/shard.startup
./$server 0 8102 ./setting/shard.startup
./$server 0 8103 ./setting/shard.startup
./$server 0 8104 ./setting/shard.startup

cd $basepath
