# 更新流程

## 上传资源
	修改Resource目录下的excel表
	点击执行 excel/_build.bat批处理, 把excel转换成config
	svn 上传资源

## Xshell 登录到打包机

### 进入打包目录

    cd /home/server/project/chess/trunk/Server/_build/linux/

![](https://i.imgur.com/xpMgPGe.png)

### 更新资源

    svn up ../../

![](https://i.imgur.com/oBI4Fl4.png)

### 打包版本

    sh make_develop.sh 0

![](https://i.imgur.com/95ITaSJ.png)

## shell更新
	
	curl -d'
	{
	"command":"version",
  	"value":"chess_develop_1.0.16.124.tar.gz'",
  	"appname":"*",
  	"apptype":"*",
  	"appid":"*",
  	"zoneid":"0"
	}' http://192.168.1.250:9999/deploy]


## 打开Postman
!> 请先安装postman工具

	输入url, 输入命令参数 点击发送即可!

![](https://i.imgur.com/EqxyOq1.png)

