#!/bin/sh

if [ ! -n "$1" ]; then
    echo "1 appname like : auth, - is allapp "
    exit 0
fi

appname=$1
if [ "$appname" = "-" ]; then
	appname="*"
fi

zoneid=$2
if [ ! -n "$zoneid" ]; then
	zoneid=0
fi

#restart 
curl -sd '{"appname":"'$appname'","apptype":"*","appid":0,"zoneid":'$zoneid',"delaytime":0}' http://127.0.0.1:9999/restart

