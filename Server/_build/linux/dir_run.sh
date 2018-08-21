
basepath=$(cd `dirname $0`; pwd)
cd /data/dir

if [ "$1" = "debug" ]; then
server=dirserverd
else
server=dirserver
fi

echo Starting Mail Cluster Server
./$server appid=100.0.121.1 log=1 startup=./setting/master.startup

echo Starting Mail Proxy Server
./$server appid=100.0.122.1 log=1 startup=./setting/proxy.startup

echo Starting Mail Shard Server
./$server appid=100.0.123.1 log=1 startup=./setting/shard.startup

cd $basepath
