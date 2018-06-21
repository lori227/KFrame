
basepath=$(cd `dirname $0`; pwd)
cd /data/platform
server=platformserver
if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Platform Server
./$server 0 11 ./setting/platform.startup
./$server 0 12 ./setting/platform.startup
cd $basepath
