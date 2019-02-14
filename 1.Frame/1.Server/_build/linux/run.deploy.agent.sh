
if [ ! -n "$1" ] ;then
	echo "please input channel"
	exit 0
fi

if [ ! -n "$2" ] ;then
	echo "please input service type"
	exit 0
fi

cd /data/deploy
./bin/kfdeploy appname=deploy apptype=agent appid=$1.12.0.1 log=1 service=1.$2 deploy=$1.11.0.1|172.31.192.96|10000
