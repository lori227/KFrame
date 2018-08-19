# redis #

- 安装redis	
		
		redis密码请自己配置
		
	- win64
	
			进入Server\_build\win64,
			解压redis-3.2.0.zip到d盘根目录,路径为d:\redis-3.2.0, 
			然后执行server_start.bat 启动redis服务器
			ps: 可以解压到随意路径, 需要修改server_start.bat脚本中的路径

	- linux
	
			yum -y install redis
			chkconfig redis on	
			修改redis配置 vim /etc/redis.conf
			绑定端口  bind 0.0.0.0
			添加密码  requirepass redis@lori227-=
			开启redis service redis start
			

		
		 
	