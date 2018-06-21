ps -ef|grep rankserver|grep -v grep|cut -c 9-15|xargs kill -9
