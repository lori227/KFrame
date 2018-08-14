
basepath=$(cd `dirname $0`; pwd)
cd /data/mailcluster

if [ "$1" = "debug" ]; then
server=mailserverd
else
server=mailserver
fi

echo Starting Mail Cluster Server
./$server appid=100.0.61.1 log=1 startup=./setting/master.startup

echo Starting Mail Proxy Server
./$server appid=100.0.62.1 log=1 startup=./setting/proxy.startup

echo Starting Mail Shard Server
./$server appid=100.0.63.1 log=1 startup=./setting/shard.startup

cd $basepath
