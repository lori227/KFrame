
basepath=$(cd `dirname $0`; pwd)
cd /data/routecluster


if [ "$1" = "debug" ]; then
server=routeserverd
else
server=routeserver
fi

echo Starting Route Cluster Server
./$server 0 30000 ./setting/master.startup

echo Starting Route Proxy Server
./$server 0 30001 ./setting/proxy.startup

echo Starting Route Shard Server
./$server 0 32001 ./setting/shard.startup

cd $basepath
