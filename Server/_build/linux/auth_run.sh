
basepath=$(cd `dirname $0`; pwd)
cd /data/auth

server=authserver
if [ "$1" = "debug" ]; then
server=authserverd
else
server=authserver
fi

echo Starting Auth Server
./$server 0 1001 ./setting/auth.startup
cd $basepath                 