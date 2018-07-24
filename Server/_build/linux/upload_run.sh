
basepath=$(cd `dirname $0`; pwd)
cd /data/upload


if [ "$1" = "debug" ]; then
server=uploadserverd
else
server=uploadserver
fi

echo Starting Upload Server
./$server 0 1 ./setting/upload.startup

cd $basepath
