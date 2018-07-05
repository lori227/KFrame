
basepath=$(cd `dirname $0`; pwd)
cd /data/datacluster

if [ "$1" = "debug" ]; then
server=dataserverd
else
server=dataserver
fi

echo Starting Data Cluster Server
./$server 0 1 ./setting/master.startup

echo Starting Data Proxy Server
./$server 0 1001 ./setting/proxy.startup
./$server 0 1002 ./setting/proxy.startup
./$server 0 1003 ./setting/proxy.startup
./$server 0 1004 ./setting/proxy.startup

echo Starting Data Shard Server
./$server 0 1101 ./setting/shard.startup
./$server 0 1102 ./setting/shard.startup
./$server 0 1103 ./setting/shard.startup
./$server 0 1104 ./setting/shard.startup
./$server 0 1105 ./setting/shard.startup
./$server 0 1106 ./setting/shard.startup
./$server 0 1107 ./setting/shard.startup
./$server 0 1108 ./setting/shard.startup
./$server 0 1109 ./setting/shard.startup
./$server 0 1110 ./setting/shard.startup
./$server 0 1111 ./setting/shard.startup
./$server 0 1112 ./setting/shard.startup

cd $basepath
