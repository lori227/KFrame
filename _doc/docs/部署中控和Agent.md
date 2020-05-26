# 部署中控

## 部署生成

	cd /home/server/frame/trunk/_build/linux
	sh build_release.sh
	sh cp.deploy.sh release

生成的部署工具包在`/data/`目录下

## 运行部署中控
选择一个服务器运行中控服务器

	tar -zxvf deploy.tar.gz
	cd deploy
	sh run.deploy.server.sh

!> `sh run.deploy.server.sh` 参数根据你channel和service而定

## 运行部署Agent
选择一个服务器运行agent服务器

	tar -zxvf deploy.tar.gz
	cd deploy
	sh run.deploy.agent.sh

!> `sh run.deploy.agent.sh` agent会自动读取数据库中的配置

	
	

