
basepath=$(cd `dirname $0`; pwd)
cd /data/deploy


if [ "$1" = "debug" ]; then
server=deployserverd
else
server=deployserver
fi

echo Starting Deploy Server
./$server 0 1 ./setting/server.startup

echo Starting Deploy Agent
./$server 0 101 ./setting/agent.startup

cd $basepath
