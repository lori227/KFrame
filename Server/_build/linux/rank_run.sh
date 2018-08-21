
basepath=$(cd `dirname $0`; pwd)
cd /data/rank


if [ "$1" = "debug" ]; then
server=rankserverd
else
server=rankserver
fi

echo Starting Rank Cluster Server
./$server appid=100.0.91.1 log=1 startup=./setting/master.startup

echo Starting Rank Proxy Server
./$server appid=100.0.92.1 log=1 startup=./setting/proxy.startup

echo Starting Rank Shard Server
./$server appid=100.0.93.1 log=1 startup=./setting/shard.startup

cd $basepath
