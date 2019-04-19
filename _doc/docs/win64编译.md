# win64编译方法

需要`vs2017 15.8.0`以上, 如没有请自行安装.  
sdk版本号为`10.0.16299.0`, 如果不是请自己项目重定向到自己的sdk版本.

## 设置vs环境变量

vs2017安装没有自动添加路径, 需要手动添加, 如果已经添加过, 请跳过以下步骤.
Win+pausebreak=>高级系统设置=>环境变量, 在系统变量下面新建:

变量名:`VS150COMNTOOLS`  
变量值: `C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\`
注意:这个是默认的安装路径, 请设置你的安装路径, 路径需要\结尾,否则编译会报错

## 编译第3方库

源码路径: `Server/KFLibrary/`
lib库路径: `Server/_lib/win64/3rd/`

进入`Server/_build/win64`目录下, 
执行`build150-library-x64.bat`, 等待编译结束,生成的结果在默认链接目录下.

!> ps: tcmalloc 和openssl 已经编译好, 如果需要请自己重新编译.

    openssl : https://www.cnblogs.com/lpxblog/p/5382653.html

!> protobuf 请自己安装cmake生成sln工程( 因为camke生成的都带绝对路径 )

	http://blog.sina.com.cn/s/blog_62949ff40102x2wo.html

# 编译框架工程

- 编译Frame.sln
	
!> 	工程包含了框架使用的lib库和最基础的插件模块  

	打开Frame.sln  
	先编译KFContrib目录( 框架封装的lib库 )  
	然后编译KFPlugin目录( 模块插件目录 )  
	最后编译KFStartup( 进程启动程序 )  
	KFTool可以不编译
			