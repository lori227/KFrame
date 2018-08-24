
basepath=$(cd `dirname $0`; pwd)
cd /data/deploy


if [ "$1" = "debug" ]; then
server=deployserverd
else
server=deployserver
fi

echo Starting Deploy Server
./$server appid=101.0.2.1 log=1 startup=./startup/deploy.server.startup

echo Starting Deploy Agent
./$server appid=101.0.3.1 log=1 startup=./startup/deploy.agent.startup

cd $basepath
