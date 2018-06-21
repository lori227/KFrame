
basepath=$(cd `dirname $0`; pwd)
cd /data/matchcluster
server=matchserver

if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Match Cluster Server
./$server 0 4 ./setting/server.startup

echo Starting Match Proxy Server
./$server 0 4001 ./setting/proxy.startup
./$server 0 4002 ./setting/proxy.startup

echo Starting Match Shard Server
./$server 0 4101 ./setting/shard.startup
./$server 0 4102 ./setting/shard.startup
./$server 0 4103 ./setting/shard.startup

cd $basepath
