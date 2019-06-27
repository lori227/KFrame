# mysql安装

## lnmp 一键安装 nginx php mysql


	wget http://soft.vpser.net/lnmp/lnmp1.5.tar.gz -cO lnmp1.5.tar.gz && tar zxf lnmp1.5.tar.gz && cd lnmp1.5 && ./install.sh lnmp


## 单独安装mysql

	wget http://repo.mysql.com/mysql57-community-release-el7.rpm

 	rpm -ivh mysql57-community-release-el7.rpm 

	yum install mysql-server

	chown -R root:root /var/lib/mysql

	service mysqld start


## 初始登录

	// 获得初始密码
	grep 'temporary password' /var/log/mysqld.log

	// 登录
	mysql -u root -p

	// 设置属性
	set global validate_password_length=0;
	set global validate_password_policy=0;

	// 创建账号
	set password for 'root'@'localhost' = password('mysql@lori227-=');
	GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' IDENTIFIED BY 'mysql@lori227-=' WITH GRANT OPTION;
	flush privileges;

	// 退出使用自己的账号登录