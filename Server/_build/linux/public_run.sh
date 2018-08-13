
basepath=$(cd `dirname $0`; pwd)
cd /data/publiccluster

if [ "$1" = "debug" ]; then
server=publicserverd
else
server=publicserverd
fi

echo Starting Public Cluster Server
./$server appid=20000 log=1 startup=./setting/master.startup

echo Starting Public Proxy Server
./$server appid=20001 log=1 startup=./setting/proxy.startup

echo Starting Public Shard Server
./$server appid=22001 log=1 startup=./setting/shard.startup

cd $basepath
