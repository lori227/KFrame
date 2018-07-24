ps -ef|grep uploadserver|grep -v grep|cut -c 9-15|xargs kill -9
