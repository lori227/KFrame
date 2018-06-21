#!/bin/sh

function copyfile()
{
	file1=$path1/$2
	file2=$path2/$2
	
	needcopyfile=0
	if [ ! -f "$file1" ];then
		needcopyfile=0
	elif [ ! -f "$file2" ];then
		needcopyfile=1	
	elif [ $1 == "1" ];then
		filetime1=`stat -c %Y $file1`
        	filetime2=`stat -c %Y $file2`

        	if [ $filetime1 -gt $filetime2 ];then
                	needcopyfile=1
        	fi
	fi
	
	if [ $needcopyfile == 1 ];then
		echo "copy $file1 $file2"
		\cp $file1 $file2
	fi
}

binpath=../../_bin/linux
respath=../../../Resource
settingpath=../../_bin/setting
deploypath=/data/rankcluster

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config

#setting
path1=$settingpath
path2=$deploypath/setting
copyfile 0 connection.network
copyfile 1 initapp.log4cxx
copyfile 1 templateapp.log4cxx

path1=$binpath/setting/rank
path2=$deploypath/setting
copyfile 1 server.network
copyfile 1 server.startup
copyfile 1 proxy.network
copyfile 1 proxy.startup
copyfile 1 shard.network
copyfile 1 shard.startup

#config
path1=$respath/config
path2=$deploypath/config
copyfile 0 redis.config
copyfile 1 rank.config
copyfile 1 option.config

#plugin
path1=$binpath/
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

path2=$deploypath/plugin
copyfile 1 KFHttpClient.so
copyfile 1 KFHttpClientd.so
copyfile 1 KFTcpServer.so
copyfile 1 KFTcpServerd.so
copyfile 1 KFTcpClient.so
copyfile 1 KFTcpClientd.so
copyfile 1 KFMessage.so
copyfile 1 KFMessaged.so
copyfile 1 KFConfig.so
copyfile 1 KFConfigd.so
copyfile 1 KFClusterServer.so
copyfile 1 KFClusterServerd.so
copyfile 1 KFClusterProxy.so
copyfile 1 KFClusterProxyd.so
copyfile 1 KFTimer.so
copyfile 1 KFTimerd.so
copyfile 1 KFClusterShard.so
copyfile 1 KFClusterShardd.so
copyfile 1 KFConnection.so
copyfile 1 KFConnectiond.so
copyfile 1 KFKernel.so
copyfile 1 KFKerneld.so
copyfile 1 KFRankProxy.so
copyfile 1 KFRankProxyd.so
copyfile 1 KFRankShard.so
copyfile 1 KFRankShardd.so