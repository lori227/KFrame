
basepath=$(cd `dirname $0`; pwd)
cd /data/groupcluster

if [ "$1" = "debug" ]; then
server=groupserverd
else
server=groupserver
fi

echo Starting Cluster Server
./$server appid=80000 log=1 startup=./setting/master.startup

echo Starting Proxy Server
./$server appid=80001 log=1 startup=./setting/proxy.startup
echo Starting Shard Server
./$server appid=82001 log=1 startup=./setting/shard.startup

cd $basepath
