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

respath=../../../Bin
deploypath=/data/zone

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config
mkdir -p $deploypath/script
mkdir -p $deploypath/startup

#setting
path1=$respath/setting
path2=$deploypath/setting
copyfile 1 ip.address
copyfile 1 bus.relation
copyfile 1 server.network
copyfile 1 cluster.setting
copyfile 1 zone.setting
copyfile 1 option.setting

path1=$respath/startup
path2=$deploypath/startup
copyfile 1 common.startup
copyfile 1 zone.gate.startup
copyfile 1 zone.login.startup
copyfile 1 zone.master.startup
copyfile 1 zone.game.startup
copyfile 1 zone.world.startup

#config
cp -f $respath/config/*.config $deploypath/config/

#script
cp -f $respath/script/* $deploypath/script/


#plugin
path1=$respath/bin
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

cp -f $deploypath/KFStartup $deploypath/zoneserver
cp -f $deploypath/KFStartupd $deploypath/zoneserverd

path1=$respath
path2=$deploypath/plugin

cp -f $path1/*.so $path2/


