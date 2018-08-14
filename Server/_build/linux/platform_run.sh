
basepath=$(cd `dirname $0`; pwd)
cd /data/platform

if [ "$1" = "debug" ]; then
server=platformserverd
else
server=platformserver
fi

echo Starting Platform Server
./$server appid=100.0.5.1 log=1 startup=./setting/platform.startup
./$server appid=100.0.5.2 log=1 startup=./setting/platform.startup
cd $basepath
