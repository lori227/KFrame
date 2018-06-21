
basepath=$(cd `dirname $0`; pwd)
cd /data/relationcluster
server=relationserver

if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Relation Cluster Server
./$server 0 7 ./setting/server.startup

echo Starting Relation Proxy Server
./$server 0 7001 ./setting/proxy.startup
./$server 0 7002 ./setting/proxy.startup

echo Starting Relation Shard Server
./$server 0 7101 ./setting/shard.startup
./$server 0 7102 ./setting/shard.startup
./$server 0 7103 ./setting/shard.startup
./$server 0 7104 ./setting/shard.startup

cd $basepath
