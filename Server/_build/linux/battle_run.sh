
basepath=$(cd `dirname $0`; pwd)
cd /data/battlecluster

if [ "$1" = "debug" ]; then
server=battleserverd
else
server=battleserver
fi

echo Starting Battle Cluster Server
./$server 0 5 ./setting/master.startup

echo Starting Battle Proxy Server
./$server 0 5001 ./setting/proxy.startup
./$server 0 5002 ./setting/proxy.startup

echo Starting Battle Shard Server
./$server 0 5101 ./setting/shard.startup
./$server 0 5102 ./setting/shard.startup
./$server 0 5103 ./setting/shard.startup
./$server 0 5104 ./setting/shard.startup

cd $basepath
