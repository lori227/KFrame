
basepath=$(cd `dirname $0`; pwd)
cd /data/auth

if [ "$1" = "debug" ]; then
server=authserverd
else
server=authserver
fi

echo Starting Auth Server
./$server appid=100.0.4.1 log=1 startup=./startup/auth.startup
cd $basepath                 