
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

build="1"
if  [ "$4" = "0" ] ;then
	build="0"
fi

	# ±‡“Î∞Ê±æ 
if  [ "$build" = "1" ] ;then
	sh build_clean.sh
	if [ "$3" = "debug" ];then
		sh build_debug.sh
	else
		sh build_release.sh
	fi
fi

# make version
svnversion=`svn info |awk 'NR==6{print $2}'`
if [ "$svnversion" = "" ];then
 svnversion=`git log -1 --pretty=format:"%h"`
fi

# resource
cd ../../
days=$(((($(date +%s ) - $(date +%s -d '20190101'))/86400) + 1));
defineversion=`cat ../3.Resource/proto/6.version.txt | cut -d "." -f 1`
clientversion=`cat ../3.Resource/proto/6.version.txt | cut -d "." -f 2`
version=$defineversion.$clientversion.$days.$svnversion
echo $version

cp -a -f ../3.Resource/config/* ./_bin/config/
cd _bin/_gcm/builder/
chmod 777 gcm_build
./gcm_build -s $svnversion -b $2 -c $1 -m $3 -v $version

cd ../../../../../
