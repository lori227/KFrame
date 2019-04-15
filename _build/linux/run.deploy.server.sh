
if [ ! -n "$1" ] ;then
	echo "please input channel"
	exit 0
fi


if [ ! -n "$2" ] ;then
	echo "please input service type"
	exit 0
fi

cd /data/deploy
./bin/kfdeploy app=deploy.server id=$1.11.0.1 log=1.0 service=1.$2
