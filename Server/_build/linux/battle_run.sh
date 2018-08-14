
basepath=$(cd `dirname $0`; pwd)
cd /data/battlecluster

if [ "$1" = "debug" ]; then
server=battleserverd
else
server=battleserver
fi

echo Starting Battle Cluster Server
./$server appid=100.0.51.1 log=1 startup=./setting/master.startup

echo Starting Battle Proxy Server
./$server appid=100.0.52.1 log=1 startup=./setting/proxy.startup

echo Starting Battle Shard Server
./$server appid=100.0.53.1 log=1 startup=./setting/shard.startup

cd $basepath
