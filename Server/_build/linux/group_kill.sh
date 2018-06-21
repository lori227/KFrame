ps -ef|grep groupserver|grep -v grep|cut -c 9-15|xargs kill -9
