
basepath=$(cd `dirname $0`; pwd)
cd /data/groupcluster

if [ "$1" = "debug" ]; then
server=groupserverd
else
server=groupserver
fi

echo Starting Cluster Server
./$server 0 80000 ./setting/master.startup

echo Starting Proxy Server
./$server 0 80001 ./setting/proxy.startup
echo Starting Shard Server
./$server 0 82001 ./setting/shard.startup

cd $basepath
