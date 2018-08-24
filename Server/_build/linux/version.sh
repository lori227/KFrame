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
respath=../../../Bin

mkdir -p $deploypath
mkdir -p $deploypath/bin
mkdir -p $deploypath/setting
mkdir -p $deploypath/startup
mkdir -p $deploypath/config
mkdir -p $deploypath/script

#setting
echo "copy setting"
cp -f $respath/setting/* $deploypath/setting/

echo "copy startup"
cp -f $respath/startup/* $deploypath/startup/

#config
echo "copy config"
cp -f $respath/config/* $deploypath/config/

#script
echo "copy script"
cp -f $respath/script/* $deploypath/script/

#plugin
echo "copy plugin"
cp -f $respath/bin/* $deploypath/bin


