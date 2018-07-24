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

if [ ! -n "$1" ]; then
    echo "please input path name"
    exit 0
fi

deploypath=/kframe/"$1"
binpath=../../_bin/linux
respath=../../../Resource
settingpath=../../_bin/setting

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config
mkdir -p $deploypath/script

#setting
path1=$settingpath
path2=$deploypath/setting
copyfile 1 bus.relation
copyfile 1 server.network
copyfile 1 initapp.log4cxx
copyfile 1 templateapp.log4cxx
copyfile 1 common.startup
copyfile 1 cluster.setting
copyfile 1 channel.setting
copyfile 1 data.setting
copyfile 0 ip.address
copyfile 0 ftp.address
copyfile 0 mysql.address
copyfile 0 redis.address
copyfile 0 zone.setting

echo "copy setting"
for file in `ls $settingpath`
do
	if [ -d $settingpath/$file ]; then
		mkdir -p $deploypath/setting/$file
		cp -f $settingpath/$file/* $deploypath/setting/$file/
	fi
done

#config
echo "copy config"
cp -f $respath/config/* $deploypath/config/

#script
echo "copy script"
cp -f $respath/script/* $deploypath/script/

#plugin
path1=$binpath
path2=$deploypath
copyfile 1 KFStartup
copyfile 1 KFStartupd

path1=$binpath
path2=$deploypath/plugin

echo "copy plugin"
cp -f $path1/*.so $path2/


