# Mongo安装

## 创建yum源

	cd /etc/yum.repos.d 
	vim mongodb-org-4.0.repo 
	添加以下内容：（我们这里使用阿里云的源）
	[mngodb-org]
	name=MongoDB Repository
	baseurl=http://mirrors.aliyun.com/mongodb/yum/redhat/7Server/mongodb-org/4.0/x86_64/
	gpgcheck=0
	enabled=1

## 安装

	yum -y install mongodb-org

## 修改配置
	
	vim /etc/mongod.conf
	
	// 监听ip
	bindIp: 172.0.0.1  改为 bindIp: 0.0.0.0

	日志路径:
	path: /data/mongo/log/mongod.log

	// 数据库路径
  	dbPath: /data/mongo/db


## 启动Mongo
	
	// 创建目录
	mkdir -p /data/mongo/db
	mkdir -p /data/mongo/log
	chmod 777 /data -R

	// 添加开机启动
	chkconfig mongod on

	// 启动
	service mongod start

## 添加管理员账户

	// 连接mongo
	mongo

	// 创建管理员
	use admin
	db.createUser({user:'admin',pwd:'admin',roles:[{role:'userAdminAnyDatabase',db:'admin'}]})

	// 创建项目账号
	use chess
	db.createUser({user:'root',pwd:'mongo=lori227-=',roles:[{role:'dbOwner',db:'chess'}]})   

## 添加需要密码验证

	修改配置
	vim /etc/mongod.conf

	// 添加密码验证
	security:
  		authorization: enabled

## 重启mongo

	service mongod restart
	
