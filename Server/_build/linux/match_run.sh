
basepath=$(cd `dirname $0`; pwd)
cd /data/matchcluster


if [ "$1" = "debug" ]; then
server=matchserverd
else
server=matchserver
fi

echo Starting Match Cluster Server
./$server appid=40000 log=1 startup=./setting/master.startup

echo Starting Match Proxy Server
./$server appid=40001 log=1 startup=./setting/proxy.startup

echo Starting Match Shard Server
./$server appid=42001 log=1 startup=./setting/shard.startup
./$server appid=42002 log=1 startup=./setting/shard.startup
./$server appid=42003 log=1 startup=./setting/shard.startup

cd $basepath
