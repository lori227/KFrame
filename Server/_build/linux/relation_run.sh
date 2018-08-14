
basepath=$(cd `dirname $0`; pwd)
cd /data/relationcluster


if [ "$1" = "debug" ]; then
server=relationserverd
else
server=relationserver
fi

echo Starting Relation Cluster Server
./$server appid=100.0.71.1 log=1 startup=./setting/master.startup

echo Starting Relation Proxy Server
./$server appid=100.0.72.1 log=1 startup=./setting/proxy.startup

echo Starting Relation Shard Server
./$server appid=100.0.73.1 log=1 startup=./setting/shard.startup

cd $basepath
