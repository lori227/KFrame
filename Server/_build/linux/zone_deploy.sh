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
deploypath=/data/zone"$2"

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config
mkdir -p $deploypath/script

#setting
path1=$settingpath
path2=$deploypath/setting
copyfile 0 connection.network
copyfile 1 initapp.log4cxx
copyfile 1 templateapp.log4cxx

path1=$settingpath/zone
path2=$deploypath/setting
copyfile 0 zone.config

copyfile 0 gate.network
copyfile 1 gate.startup
copyfile 1 login.network
copyfile 1 login.startup
copyfile 1 master.network
copyfile 1 master.startup
copyfile 0 proxy.network
copyfile 1 proxy.startup
copyfile 1 game.network
copyfile 1 game.startup
copyfile 1 world.network
copyfile 1 world.startup

#config
\cp $respath/config/*.config $deploypath/config/

#script
\cp $respath/script/* $deploypath/script/


#plugin
path1=$binpath
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

path1=$binpath
path2=$deploypath/plugin

\cp $path1/*.so $path2/


