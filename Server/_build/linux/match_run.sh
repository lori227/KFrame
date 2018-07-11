
basepath=$(cd `dirname $0`; pwd)
cd /data/matchcluster


if [ "$1" = "debug" ]; then
server=matchserverd
else
server=matchserver
fi

echo Starting Match Cluster Server
./$server 0 40000 ./setting/master.startup

echo Starting Match Proxy Server
./$server 0 40001 ./setting/proxy.startup

echo Starting Match Shard Server
./$server 0 42001 ./setting/shard.startup
./$server 0 42002 ./setting/shard.startup
./$server 0 42003 ./setting/shard.startup

cd $basepath
