
if [ ! -n "$1" ]; then
    	echo "please input app channel : 101"
    	exit 0
fi

if  [ ! -n "$2" ] ;then
	echo "please input build type : debug/release"
	exit 0
fi


if [ "$1" = "100" ];then
	branch=0
elif [ "$1" = "101" ];then
	branch=1
else
	branch=2
fi
echo $branch

svnversion=`svn info |awk 'NR==6{print $2}'`
if [ "$svnversion" = "" ];then
 svnversion=`git log -1 --pretty=format:"%h"`
fi

echo $svnversion

# ±àÒë°æ±¾ 
sh build_clean.sh
if [ "$2" = "debug" ];then
	sh build_debug.sh
else
	sh build_release.sh
fi

# ¿½±´´ò°ü£¬´æµµ°æ±¾
cd ../../../
\cp -f ./Resource/config/* ./Bin/config/
cd Bin/tool/gcm/builder/
chmod 777 gcm_build
./gcm_build -s $svnversion -b $branch -c $1 -m $2

cd ../../../../../../../
