
basepath=$(cd `dirname $0`; pwd)
cd /data/routecluster
server=routeserver

if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Route Cluster Server
./$server 0 3 ./setting/server.startup

echo Starting Route Proxy Server
./$server 0 3001 ./setting/proxy.startup
./$server 0 3002 ./setting/proxy.startup

echo Starting Route Shard Server
./$server 0 3101 ./setting/shard.startup
./$server 0 3102 ./setting/shard.startup
./$server 0 3103 ./setting/shard.startup
./$server 0 3104 ./setting/shard.startup

cd $basepath
