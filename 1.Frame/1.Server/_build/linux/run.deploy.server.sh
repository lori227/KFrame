
if [ ! -n "$1" ] ;then
	echo "please input channel"
	exit 0
fi


if [ ! -n "$2" ] ;then
	echo "please input service type"
	exit 0
fi

cd /data/deploy
./bin/deployserver appname=deploy apptype=server appid=$1.11.0.1 log=1.1 service=1.$2