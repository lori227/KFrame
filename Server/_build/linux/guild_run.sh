
basepath=$(cd `dirname $0`; pwd)
cd /data/guildcluster
server=guildserver

if [ "$1" = "debug" ]; then
server=guildserverd
else
server=guildserver
fi

echo Starting Cluster Server
./$server 0 90000 ./setting/master.startup

echo Starting Proxy Server
./$server 0 90001 ./setting/proxy.startup

echo Starting Shard Server
./$server 0 92001 ./setting/shard.startup

cd $basepath
