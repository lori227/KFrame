
basepath=$(cd `dirname $0`; pwd)
cd /data/upload


if [ "$1" = "debug" ]; then
server=uploadserverd
else
server=uploadserver
fi

echo Starting Upload Server
./$server appid=101.0.1.1 log=1 startup=./startup/deploy.upload.startup

cd $basepath
