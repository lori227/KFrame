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
curl -sd '{"command":"restart","value":"0","appname":"'$appname'","apptype":"*","appid":"*","zoneid":'$zoneid'}' http://123.206.102.191:9999/deploy

