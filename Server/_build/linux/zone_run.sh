
basepath=$(cd `dirname $0`; pwd)
cd /data/zone

if [ "$1" = "debug" ]; then
server=zoneserverd
else
server=zoneserver
fi

echo Starting Master Server
./$server 0 100 ./setting/master.startup

echo Starting World Server
./$server 0 200 ./setting/world.startup

echo Starting Gate Server
./$server 0 301 ./setting/gate.startup
./$server 0 302 ./setting/gate.startup

echo Starting Login Server
./$server 0 401 ./setting/login.startup
./$server 0 402 ./setting/login.startup

echo Starting Proxy Server
./$server 0 501 ./setting/proxy.startup
./$server 0 502 ./setting/proxy.startup

echo Starting Game Server
./$server 0 601 ./setting/game.startup
./$server 0 602 ./setting/game.startup


cd $basepath

