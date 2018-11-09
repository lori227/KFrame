# linux编译方法

!> 以下方式仅在CentOS 7+编译通过, 其他版本linux请自行测试

# 安装编译环境

如已经安装完成, 跳过一下步骤.

1. 安装openssl

```shell
yum -y install openssl-devel
```

2. 安装mysql

```shell
yum -y install mysql-devel
```

3. 安装cmake-3.10.1

```shell
cd Server/_build/linux/tool
tar zxvf cmake-3.10.1.tar.gz
cd cmake-3.10.1
./bootstrap
./configure --prefix=/usr
gmake
gmake install
```

## 编译第3方库

源码路径: `Server/KFLibrary/`  
lib库路径: `Server/_lib/linux/3rd/`  
如果你想使用仓库已经编译好的库文件, 跳过以下步骤.  
进入`Server/_build/linux`目录下,  
执行`build_library.sh`, 等待编译结束,生成的结果在默认链接目录下

## 编译框架工程

进入`Server/_build/linux`目录下,

```shell
#编译debug版本
./build_debug.sh

#编译Release版本
./build_release.sh

#清空编译文件
./build_clean.sh
```