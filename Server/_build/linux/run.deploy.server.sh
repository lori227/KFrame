
if  [ ! -n "$1" ] ;then
	echo "please input channel"
	exit 0
fi


if  [ ! -n "$2" ] ;then
	echo "please input build type : debug/release"
	exit 0
fi

if [ "$2" = "debug" ]; then
server=deployserverd
else
server=deployserver
fi

cd /data/deploy
./bin/$server appid=$1.0.2.1 log=1.1 startup=./startup/deploy.server.startup
