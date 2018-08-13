
basepath=$(cd `dirname $0`; pwd)
cd /data/logcluster

if [ "$1" = "debug" ]; then
server=logserverd
else
server=logserver
fi

echo Starting Log Cluster Server
./$server appid=110000 log=1 startup=./setting/master.startup

echo Starting Log Proxy Server
./$server appid=110001 log=1 startup=./setting/proxy.startup

echo Starting Log Shard Server
./$server appid=112001 log=1 startup=./setting/shard.startup

cd $basepath
