
basepath=$(cd `dirname $0`; pwd)
cd /data/logcluster

if [ "$1" = "debug" ]; then
server=logserverd
else
server=logserver
fi

echo Starting Log Cluster Server
./$server 0 110000 ./setting/master.startup

echo Starting Log Proxy Server
./$server 0 110001 ./setting/proxy.startup

echo Starting Log Shard Server
./$server 0 112001 ./setting/shard.startup

cd $basepath
