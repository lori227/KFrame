# Redis安装

## win64

1.Frame\4.Document\\#001 - tool  
解压redis-3.2.0.zip到d盘根目录,路径为d:\redis-3.2.0  
然后执行server_start.bat 启动redis服务器  

?> 可以解压到随意路径, 需要修改server_start.bat脚本中的路径

## linux

```shell
yum -y install redis
chkconfig redis on

#修改redis配置
vim /etc/redis.conf
#绑定端口
bind 0.0.0.0

#设置密码
requirepass redis@lori227-=

#开启redis
service redis start
```

!> redis密码请自己配置