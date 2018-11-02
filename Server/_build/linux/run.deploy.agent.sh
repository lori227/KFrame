
if [ ! -n "$1" ] ;then
	echo "please input channel"
	exit 0
fi

if [ ! -n "$2" ] ;then
	echo "please input service type"
	exit 0
fi

if [ ! -n "$3" ] ;then
	echo "please input agent instance"
	exit 0
fi

cd /data/deploy
./bin/deployserver appid=$1.0.3.$3 log=1.1 service=1.$2 startup=./startup/deploy.agent.startup
