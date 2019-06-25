
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

svn up ../../../Resource

build="1"
uptype = 1
if  [ "$4" = "0" ] ;then
	build="0"
	uptype = 3
fi

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
days=$(((($(date +%s ) - $(date +%s -d '20190401'))/86400) + 1));
defineversion=`cat ../Resource/proto/6.version.txt | cut -d "." -f 1`
clientversion=`cat ../Resource/proto/6.version.txt | cut -d "." -f 2`
version=$defineversion.$clientversion.$days.$svnversion
echo $version

# config
mkdir -p _bin/config
cp -f ../Resource/config/*.xml _bin/
cp -f ../Resource/config/server/*.xml _bin/

#script
mkdir -p _bin/script
cp -rf ../Resource/script/ _bin/

cd _bin/_gcm/builder/
chmod 777 gcm_build
./gcm_build -p "chess" -s $svnversion -b $2 -c $1 -m $3 -v $version -n 1.2 -t $uptype

cd ../../../../../
