
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

# ±‡“Î∞Ê±æ 
sh build_clean.sh
if [ "$3" = "debug" ];then
	sh build_debug.sh
else
	sh build_release.sh
fi

# make version
svnversion=`svn info |awk 'NR==6{print $2}'`
if [ "$svnversion" = "" ];then
 svnversion=`git log -1 --pretty=format:"%h"`
fi

# resource
cd ../../../
days=$(((($(date +%s ) - $(date +%s -d '20181101'))/86400) + 1));
clientversion=`cat ./Resource/protocol/version.txt | cut -d "." -f 1`
battleversion=`cat ./Resource/protocol/version.txt | cut -d "." -f 2`
version=$clientversion.$battleversion.$days.$svnversion
echo $version

\cp -f ./Resource/config/* ./Bin/config/
cd Bin/tool/gcm/builder/
chmod 777 gcm_build
./gcm_build -s $svnversion -b $2 -c $1 -m $3 -v $version

cd ../../../../../../../
