
basepath=$(cd `dirname $0`; pwd)
cd /data/rankcluster
server=rankserver

if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Cluster Server
./$server 0 10 ./setting/server.startup

echo Starting Proxy Server
./$server 0 10001 ./setting/proxy.startup
./$server 0 10002 ./setting/proxy.startup

echo Starting Shard Server
./$server 0 10101 ./setting/shard.startup
./$server 0 10102 ./setting/shard.startup
./$server 0 10103 ./setting/shard.startup
./$server 0 10104 ./setting/shard.startup

cd $basepath
