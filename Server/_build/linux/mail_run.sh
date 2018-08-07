
basepath=$(cd `dirname $0`; pwd)
cd /data/mailcluster

if [ "$1" = "debug" ]; then
server=mailserverd
else
server=mailserver
fi

echo Starting Mail Cluster Server
./$server appid=60000 log=1 startup=./setting/master.startup

echo Starting Mail Proxy Server
./$server appid=60001 log=1 startup=./setting/proxy.startup

echo Starting Mail Shard Server
./$server appid=62001 log=1 startup=./setting/shard.startup

cd $basepath
