
basepath=$(cd `dirname $0`; pwd)
cd /data/launch

server=launchserver
\cp KFStartupd $server

echo Starting Launch
launchid=9999
./$server 0 $launchid ./setting/launch.startup

cd $basepath

