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
		cp -f $file1 $file2
	fi
}

binpath=../../_bin/linux
respath=../../../Resource
settingpath=../../_bin/setting
deploypath=/data/groupcluster

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config

#setting
path1=$settingpath
path2=$deploypath/setting
copyfile 1 ip.address
copyfile 1 bus.relation
copyfile 1 server.network
copyfile 1 initapp.log4cxx
copyfile 1 templateapp.log4cxx

path2=$deploypath/config
copyfile 1 cluster.config

#config
path1=$respath/config
path2=$deploypath/config
copyfile 1 group.config
copyfile 1 kernel.config
copyfile 1 class.config

path1=$settingpath/group
path2=$deploypath/setting
copyfile 1 master.startup
copyfile 1 proxy.startup
copyfile 1 shard.startup


#plugin
path1=$binpath
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

cp -f $deploypath/KFStartup $deploypath/groupserver
cp -f $deploypath/KFStartupd $deploypath/groupserverd

path2=$deploypath/plugin
copyfile 1 KFBus.so
copyfile 1 KFBusd.so
copyfile 1 KFIpAddress.so
copyfile 1 KFIpAddressd.so
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
copyfile 1 KFGroupProxy.so
copyfile 1 KFGroupProxyd.so
copyfile 1 KFGroupShard.so
copyfile 1 KFGroupShardd.so
copyfile 1 KFKernel.so
copyfile 1 KFKerneld.so
copyfile 1 KFDisplayd.so
copyfile 1 KFDisplay.so
copyfile 1 KFDeployClient.so
copyfile 1 KFDeployClientd.so
copyfile 1 KFDeployCommand.so
copyfile 1 KFDeployCommandd.so