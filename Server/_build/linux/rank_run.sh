
basepath=$(cd `dirname $0`; pwd)
cd /data/rankcluster


if [ "$1" = "debug" ]; then
server=rankserverd
else
server=rankserver
fi

echo Starting Rank Cluster Server
./$server 0 90000 ./setting/master.startup

echo Starting Rank Proxy Server
./$server 0 90001 ./setting/proxy.startup

echo Starting Rank Shard Server
./$server 0 92001 ./setting/shard.startup

cd $basepath
