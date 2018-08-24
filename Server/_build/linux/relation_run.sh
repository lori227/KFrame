
basepath=$(cd `dirname $0`; pwd)
cd /data/relation


if [ "$1" = "debug" ]; then
server=relationserverd
else
server=relationserver
fi

echo Starting Relation Cluster Server
./$server appid=100.0.71.1 log=1 startup=./startup/relation.master.startup

echo Starting Relation Proxy Server
./$server appid=100.0.72.1 log=1 startup=./startup/relation.proxy.startup

echo Starting Relation Shard Server
./$server appid=100.0.73.1 log=1 startup=./startup/relation.shard.startup

cd $basepath
