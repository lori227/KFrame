
basepath=$(cd `dirname $0`; pwd)
cd /data/routecluster


if [ "$1" = "debug" ]; then
server=routeserverd
else
server=routeserver
fi

echo Starting Route Cluster Server
./$server appid=100.0.31.1 log=1 startup=./setting/master.startup

echo Starting Route Proxy Server
./$server appid=100.0.32.1 log=1 startup=./setting/proxy.startup

echo Starting Route Shard Server
./$server appid=100.0.33.1 log=1 startup=./setting/shard.startup

cd $basepath
