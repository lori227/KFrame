# Jenkins 


## 安装jenkins

	yum install -y java

	wget -O /etc/yum.repos.d/jenkins.repo http://pkg.jenkins-ci.org/redhat/jenkins.repo
	rpm --import https://jenkins-ci.org/redhat/jenkins-ci.org.key

	yum install -y jenkins

	service jenkins start

!> 初始密码在：/var/lib/jenkins/secrets/initialAdminPassword 


## 安装插件

	安装 Publish Over SSH 插件

## 配置远程用户
	
	系统管理=>系统设置=>Publish over SSH 添加ssh 服务器

## 添加任务

添加远程ssh命令
send files or execute commands over ssh


	cd /home/server/project/chess/trunk
	svn up Resource/
	svn up Server
	cd Server/_build/linux
	sh make_develop.sh


	svnversion=`svn info |awk 'NR==6{print $2}'`
	cd ../../
	days=$(((($(date +%s ) - $(date +%s -d '20190401'))/86400) + 1));
	defineversion=`cat ../Resource/proto/6.version.txt | cut -d "." -f 1`
	clientversion=`cat ../Resource/proto/6.version.txt | cut -d "." -f 2`
	version=$defineversion.$clientversion.$days.$svnversion
	echo $version

	curl -d'
	{
	"command":"version",
  	"value":"chess_develop_'$version.tar.gz'",
  	"appname":"*",
  	"apptype":"*",
  	"appid":"*",
  	"zoneid":"0"
	}' http://192.168.1.250:9999/deploy


## 一键构建任务 ok!