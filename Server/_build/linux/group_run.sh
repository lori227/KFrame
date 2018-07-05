
basepath=$(cd `dirname $0`; pwd)
cd /data/groupcluster

if [ "$1" = "debug" ]; then
server=groupserverd
else
server=groupserver
fi

echo Starting Cluster Server
./$server 0 8 ./setting/master.startup

echo Starting Proxy Server
./$server 0 8001 ./setting/proxy.startup
./$server 0 8002 ./setting/proxy.startup

echo Starting Shard Server
./$server 0 8101 ./setting/shard.startup
./$server 0 8102 ./setting/shard.startup
./$server 0 8103 ./setting/shard.startup
./$server 0 8104 ./setting/shard.startup

cd $basepath
