
server=zone"$2"server
echo $server
ps -ef|grep "$server"|grep -v grep|cut -c 9-15|xargs kill -9
