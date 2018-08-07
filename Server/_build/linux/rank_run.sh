
basepath=$(cd `dirname $0`; pwd)
cd /data/rankcluster


if [ "$1" = "debug" ]; then
server=rankserverd
else
server=rankserver
fi

echo Starting Rank Cluster Server
./$server appid=90000 log=1 startup=./setting/master.startup

echo Starting Rank Proxy Server
./$server appid=90001 log=1 startup=./setting/proxy.startup

echo Starting Rank Shard Server
./$server appid=92001 log=1 startup=./setting/shard.startup

cd $basepath
