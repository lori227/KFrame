
basepath=$(cd `dirname $0`; pwd)
cd /data/zone

if [ "$1" = "debug" ]; then
server=zoneserverd
else
server=zoneserver
fi

echo Starting Master Server
./$server appid=100.1.241.1 log=1 startup=./setting/master.startup

echo Starting World Server
./$server appid=100.1.242.1 log=1 startup=./setting/world.startup

echo Starting Gate Server
./$server appid=100.1.243.1 log=1 startup=./setting/gate.startup

echo Starting Login Server
./$server appid=100.1.244.1 log=1 startup=./setting/login.startup

echo Starting Game Server
./$server appid=100.1.246.1 log=1 startup=./setting/game.startup


cd $basepath

