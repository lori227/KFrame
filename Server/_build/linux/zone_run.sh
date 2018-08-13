
basepath=$(cd `dirname $0`; pwd)
cd /data/zone

if [ "$1" = "debug" ]; then
server=zoneserverd
else
server=zoneserver
fi

echo Starting Master Server
./$server appid=100 log=1 startup=./setting/master.startup

echo Starting World Server
./$server appid=200 log=1 startup=./setting/world.startup

echo Starting Gate Server
./$server appid=301 log=1 startup=./setting/gate.startup
./$server appid=302 log=1 startup=./setting/gate.startup

echo Starting Login Server
./$server appid=401 log=1 startup=./setting/login.startup
./$server appid=402 log=1 startup=./setting/login.startup

echo Starting Proxy Server
./$server appid=501 log=1 startup=./setting/proxy.startup
./$server appid=502 log=1 startup=./setting/proxy.startup

echo Starting Game Server
./$server appid=601 log=1 startup=./setting/game.startup
./$server appid=602 log=1 startup=./setting/game.startup


cd $basepath

