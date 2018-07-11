
basepath=$(cd `dirname $0`; pwd)
cd /data/battlecluster

if [ "$1" = "debug" ]; then
server=battleserverd
else
server=battleserver
fi

echo Starting Battle Cluster Server
./$server 0 50000 ./setting/master.startup

echo Starting Battle Proxy Server
./$server 0 50001 ./setting/proxy.startup

echo Starting Battle Shard Server
./$server 0 52001 ./setting/shard.startup

cd $basepath
