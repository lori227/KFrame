
basepath=$(cd `dirname $0`; pwd)
cd /data/guildcluster
server=guildserver

if [ "$1" = "debug" ]; then
server=guildserverd
else
server=guildserver
fi

echo Starting Cluster Server
./$server appid=90000 log=1 startup=./setting/master.startup

echo Starting Proxy Server
./$server appid=90001 log=1 startup=./setting/proxy.startup

echo Starting Shard Server
./$server appid=92001 log=1 startup=./setting/shard.startup

cd $basepath
