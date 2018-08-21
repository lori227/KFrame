
basepath=$(cd `dirname $0`; pwd)
cd /data/data

if [ "$1" = "debug" ]; then
server=dataserverd
else
server=dataserver
fi

echo Starting Data Cluster Server
./$server appid=100.0.11.1 log=1 startup=./setting/master.startup

echo Starting Data Proxy Server
./$server appid=100.0.12.1 log=1 startup=./setting/proxy.startup

echo Starting Data Shard Server
./$server appid=100.0.13.1 log=1 startup=./setting/shard.startup
cd $basepath
