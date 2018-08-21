
basepath=$(cd `dirname $0`; pwd)
cd /data/group

if [ "$1" = "debug" ]; then
server=groupserverd
else
server=groupserver
fi

echo Starting Cluster Server
./$server appid=100.0.81.1 log=1 startup=./setting/master.startup

echo Starting Proxy Server
./$server appid=100.0.82.1 log=1 startup=./setting/proxy.startup
echo Starting Shard Server
./$server appid=100.0.83.1 log=1 startup=./setting/shard.startup

cd $basepath
