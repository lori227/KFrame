
basepath=$(cd `dirname $0`; pwd)
cd /data/publiccluster

if [ "$1" = "debug" ]; then
server=publicserverd
else
server=publicserverd
fi

echo Starting Public Cluster Server
./$server 0 20000 ./setting/master.startup

echo Starting Public Proxy Server
./$server 0 20001 ./setting/proxy.startup

echo Starting Public Shard Server
./$server 0 22001 ./setting/shard.startup

cd $basepath
