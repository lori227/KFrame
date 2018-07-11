
basepath=$(cd `dirname $0`; pwd)
cd /data/datacluster

if [ "$1" = "debug" ]; then
server=dataserverd
else
server=dataserver
fi

echo Starting Data Cluster Server
./$server 0 10000 ./setting/master.startup

echo Starting Data Proxy Server
./$server 0 10001 ./setting/proxy.startup

echo Starting Data Shard Server
./$server 0 12001 ./setting/shard.startup
./$server 0 12002 ./setting/shard.startup
./$server 0 12003 ./setting/shard.startup
./$server 0 12004 ./setting/shard.startup
./$server 0 12005 ./setting/shard.startup
./$server 0 12006 ./setting/shard.startup
./$server 0 12007 ./setting/shard.startup
./$server 0 12008 ./setting/shard.startup
./$server 0 12009 ./setting/shard.startup
./$server 0 12010 ./setting/shard.startup

cd $basepath
