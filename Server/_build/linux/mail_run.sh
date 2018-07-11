
basepath=$(cd `dirname $0`; pwd)
cd /data/mailcluster

if [ "$1" = "debug" ]; then
server=mailserverd
else
server=mailserver
fi

echo Starting Mail Cluster Server
./$server 0 60000 ./setting/master.startup

echo Starting Mail Proxy Server
./$server 0 60001 ./setting/proxy.startup

echo Starting Mail Shard Server
./$server 0 62001 ./setting/shard.startup

cd $basepath
