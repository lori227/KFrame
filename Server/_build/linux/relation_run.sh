
basepath=$(cd `dirname $0`; pwd)
cd /data/relationcluster


if [ "$1" = "debug" ]; then
server=relationserverd
else
server=relationserver
fi

echo Starting Relation Cluster Server
./$server 0 70000 ./setting/master.startup

echo Starting Relation Proxy Server
./$server 0 70001 ./setting/proxy.startup

echo Starting Relation Shard Server
./$server 0 72001 ./setting/shard.startup

cd $basepath
