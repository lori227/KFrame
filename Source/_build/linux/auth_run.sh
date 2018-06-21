
basepath=$(cd `dirname $0`; pwd)
cd /data/auth

server=authserver
if [ "$1" = "debug" ]; then
\cp KFStartupd $server
else
\cp KFStartup $server
fi

echo Starting Auth Server
./$server 0 1 ./setting/auth.startup
cd $basepath
