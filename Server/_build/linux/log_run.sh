
basepath=$(cd `dirname $0`; pwd)
cd /data/logcluster

if [ "$1" = "debug" ]; then
server=logserverd
else
server=logserver
fi

echo Starting Log Cluster Server
./$server appid=100.0.111.1 log=1 startup=./setting/master.startup

echo Starting Log Proxy Server
./$server appid=100.0.112.1 log=1 startup=./setting/proxy.startup

echo Starting Log Shard Server
./$server appid=100.0.113.1 log=1 startup=./setting/shard.startup

cd $basepath
