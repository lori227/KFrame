
basepath=$(cd `dirname $0`; pwd)
cd /data/publiccluster
server=publicserver
if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Public Cluster Server
./$server 0 2 ./setting/server.startup

echo Starting Public Proxy Server
./$server 0 2001 ./setting/proxy.startup
./$server 0 2002 ./setting/proxy.startup

echo Starting Public Shard Server
./$server 0 2101 ./setting/shard.startup
./$server 0 2102 ./setting/shard.startup
./$server 0 2103 ./setting/shard.startup
./$server 0 2104 ./setting/shard.startup

cd $basepath
