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

deploypath=/data/launch
binpath=../../_bin
respath=../../../Resource

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config
#mkdir -p $deploypath/script

#Bin
path1=$binpath/linux
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

#setting
path1=$binpath/setting
path2=$deploypath/setting
copyfile 1 kernel.config
copyfile 1 redis.config
copyfile 0 connection.network
copyfile 1 initapp.log4cxx
copyfile 1 templateapp.log4cxx

path1=$binpath/setting/launch
path2=$deploypath/setting
#copyfile 0 zone.config

copyfile 1 launch.network
copyfile 1 launch.startup

#config
\cp $respath/config/*.config $deploypath/config/

#script
#\cp $respath/script/* $deploypath/script/


#plugin
path1=$binpath/linux
path2=$deploypath/plugin

\cp $path1/*.so $path2/


