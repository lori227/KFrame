
basepath=$(cd `dirname $0`; pwd)
cd /data/upload


if [ "$1" = "debug" ]; then
server=uploadserverd
else
server=uploadserver
fi

echo Starting Upload Server
./$server appid=1 log=1 startup=./setting/upload.startup

cd $basepath
