
basepath=$(cd `dirname $0`; pwd)
cd /data/auth

if [ "$1" = "debug" ]; then
server=authserverd
else
server=authserver
fi

echo Starting Auth Server
./$server appid=1001 log=1 startup=./setting/auth.startup
cd $basepath                 