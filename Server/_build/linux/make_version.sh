
if [ ! -n "$1" ]; then
    	echo "please input app channel : 1"
    	exit 0
fi

if  [ ! -n "$2" ] ;then
	echo "please input branch : develop/online/steam"
	exit 0
fi

if  [ ! -n "$3" ] ;then
	echo "please input build type : debug/release"
	exit 0
fi

svnversion=`svn info |awk 'NR==6{print $2}'`
if [ "$svnversion" = "" ];then
 svnversion=`git log -1 --pretty=format:"%h"`
fi

echo $svnversion

# ±àÒë°æ±¾ 
sh build_clean.sh
if [ "$3" = "debug" ];then
	sh build_debug.sh
else
	sh build_release.sh
fi

# ¿½±´´ò°ü£¬´æµµ°æ±¾
cd ../../../
\cp -f ./Resource/config/* ./Bin/config/
cd Bin/tool/gcm/builder/
chmod 777 gcm_build
./gcm_build -s $svnversion -b $2 -c $1 -m $3

cd ../../../../../../../
