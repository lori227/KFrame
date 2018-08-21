#!/bin/sh

function copyfile()
{
	file1=$path1/$2
	file2=$path2/$2
	if [ -n "$3" ]; then
		file2=$path2/$3
	fi

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

if [ ! -n "$3" ]; then
    echo "please input path name"
    exit 0
fi

versionpath=/kframe/$1
deploypath=/data/upload/deploy/group/linux

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config

#setting
path1=$versionpath/setting
path2=$deploypath/setting
copyfile 1 ip.address
copyfile 1 bus.relation
copyfile 1 server.network
copyfile 1 common.startup
copyfile 1 cluster.setting
copyfile 1 kernel.setting

path1=$versionpath/setting/group
copyfile 1 master.startup
copyfile 1 proxy.startup
copyfile 1 shard.startup

#config
path1=$versionpath/config
path2=$deploypath/config
copyfile 1 group.config
copyfile 1 class.config

#plugin
path1=$versionpath
path2=$deploypath
copyfile 1 KFStartup groupserver
copyfile 1 KFStartupd groupserverd

path1=$versionpath/plugin
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
copyfile 1 KFClusterMaster.so
copyfile 1 KFClusterMasterd.so
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
copyfile 1 KFClusterClientd.so
copyfile 1 KFClusterClient.so
copyfile 1 KFLogClientd.so
copyfile 1 KFLogClient.so

#upload to ftp
curl -sd '{"ftpid":'$2',"apppath":"group"}' $3
