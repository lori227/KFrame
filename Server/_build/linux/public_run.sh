
basepath=$(cd `dirname $0`; pwd)
cd /data/publiccluster

if [ "$1" = "debug" ]; then
server=publicserverd
else
server=publicserverd
fi

echo Starting Public Cluster Server
./$server appid=100.0.21.1 log=1 startup=./setting/master.startup

echo Starting Public Proxy Server
./$server appid=100.0.22.1 log=1 startup=./setting/proxy.startup

echo Starting Public Shard Server
./$server appid=100.0.23.1 log=1 startup=./setting/shard.startup

cd $basepath
