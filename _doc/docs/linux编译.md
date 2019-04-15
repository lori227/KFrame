# linux编译方法

!> 以下方式仅在CentOS 7+编译通过, 其他版本linux请自行测试

# 安装编译环境
1. 安装cmake-3.10.1

```shell
cd 1.Frame\4.Document\#001 - tool
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

进入`Server/_build/linux`目录下,  
执行`sh build_library.sh`, 等待编译结束,生成的结果在默认链接目录下

## 编译框架工程

进入`Server/_build/linux`目录下,

```shell
#编译debug版本
sh build_debug.sh

#编译Release版本
sh build_release.sh

#清空编译文件
sh build_clean.sh
```