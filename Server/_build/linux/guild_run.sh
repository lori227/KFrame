
basepath=$(cd `dirname $0`; pwd)
cd /data/guild

if [ "$1" = "debug" ]; then
server=guildserverd
else
server=guildserver
fi

echo Starting Cluster Server
./$server appid=100.0.101.1 log=1 startup=./setting/master.startup

echo Starting Proxy Server
./$server appid=100.0.102.1 log=1 startup=./setting/proxy.startup

echo Starting Shard Server
./$server appid=100.0.103.1 log=1 startup=./setting/shard.startup

cd $basepath
