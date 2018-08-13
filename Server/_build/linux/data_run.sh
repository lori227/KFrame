
basepath=$(cd `dirname $0`; pwd)
cd /data/datacluster

if [ "$1" = "debug" ]; then
server=dataserverd
else
server=dataserver
fi

echo Starting Data Cluster Server
./$server appid=10000 log=1 startup=./setting/master.startup

echo Starting Data Proxy Server
./$server appid=10001 log=1 startup=./setting/proxy.startup

echo Starting Data Shard Server
./$server appid=12001 log=1 startup=./setting/shard.startup
./$server appid=12002 log=1 startup=./setting/shard.startup
./$server appid=12003 log=1 startup=./setting/shard.startup
./$server appid=12004 log=1 startup=./setting/shard.startup
./$server appid=12005 log=1 startup=./setting/shard.startup
./$server appid=12006 log=1 startup=./setting/shard.startup
./$server appid=12007 log=1 startup=./setting/shard.startup
./$server appid=12008 log=1 startup=./setting/shard.startup
./$server appid=12009 log=1 startup=./setting/shard.startup
./$server appid=12010 log=1 startup=./setting/shard.startup

cd $basepath
