
basepath=$(cd `dirname $0`; pwd)
cd /data/guildcluster
server=guildserver

if [ "$1" = "debug" ]; then
server=guildserverd
else
server=guildserver
fi

echo Starting Cluster Server
./$server 0 9 ./setting/master.startup

echo Starting Proxy Server
./$server 0 9001 ./setting/proxy.startup
./$server 0 9002 ./setting/proxy.startup

echo Starting Shard Server
./$server 0 9101 ./setting/shard.startup
./$server 0 9102 ./setting/shard.startup
./$server 0 9103 ./setting/shard.startup
./$server 0 9104 ./setting/shard.startup

cd $basepath
