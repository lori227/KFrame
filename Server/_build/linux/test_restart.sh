#!/bin/sh

appname=$1
if[ "$appname" = "-"]; then
	appname="*"
fi

zoneid=$2
if[ ! -n "$zoneid" ]; then
	zoneid=0
fi


echo $appname
echo $zoneid
#restart 
curl -sd '{"appname":'$1',"apptype":"*","appid":0,"zoneid":'$zoneid',"delaytime":0}' http://127.0.0.1:9999/restart

