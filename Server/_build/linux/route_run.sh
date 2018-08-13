
basepath=$(cd `dirname $0`; pwd)
cd /data/routecluster


if [ "$1" = "debug" ]; then
server=routeserverd
else
server=routeserver
fi

echo Starting Route Cluster Server
./$server appid=30000 log=1 startup=./setting/master.startup

echo Starting Route Proxy Server
./$server appid=30001 log=1 startup=./setting/proxy.startup

echo Starting Route Shard Server
./$server appid=32001 log=1 startup=./setting/shard.startup

cd $basepath
