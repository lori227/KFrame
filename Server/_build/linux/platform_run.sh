
basepath=$(cd `dirname $0`; pwd)
cd /data/platform

if [ "$1" = "debug" ]; then
server=platformserverd
else
server=platformserver
fi

echo Starting Platform Server
./$server 0 11 ./setting/platform.startup
./$server 0 12 ./setting/platform.startup
cd $basepath
