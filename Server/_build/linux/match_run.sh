
basepath=$(cd `dirname $0`; pwd)
cd /data/match


if [ "$1" = "debug" ]; then
server=matchserverd
else
server=matchserver
fi

echo Starting Match Cluster Server
./$server appid=100.0.31.1 log=1 startup=./setting/master.startup

echo Starting Match Proxy Server
./$server appid=100.0.42.1 log=1 startup=./setting/proxy.startup

echo Starting Match Shard Server
./$server appid=100.0.43.1 log=1 startup=./setting/shard.startup

cd $basepath
