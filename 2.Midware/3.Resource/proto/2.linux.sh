#! /bin/sh 

chmod 777 4.protoc
export LD_LIBRARY_PATH=/usr/local/lib

for file in `ls *.proto`  
do  
	echo $file  
	./4.protoc -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ $file
done  

\cp *.pb.* ../../1.Server/KFContrib/KFProtocol/
rm -f *.pb.*

# make proto version
function makeversion()
{
	oldmd5client=`cat 5.md5list | grep "$1" | cut -d ":" -f 2`
	newmd5client=`md5sum ./$1 | awk '{print $1}'`
			
	declare -i version=`cat 5.md5list | grep "$1" | cut -d ":" -f 3`
	if [ $oldmd5client != $newmd5client ];then
		declare -i newversion=$version+1
		sed -i "s/$oldmd5client:$version/$newmd5client:$newversion/g" 5.md5list
		version=$newversion
	fi

	return $version
}

# client proto
makeversion KFDefineMessage.proto
defineversion=$?
echo "define version=$defineversion"

makeversion KFMidwareClientMessage.proto
clientversion=$?
echo "client version=$clientversion"

# write in version file
echo "$defineversion.$clientversion.0.0" > 6.version.txt

#svn 
# svn ci -m "proto version" 5.md5list 6.version.txt

#git
git commit -m "proto version" 5.md5list 6.version.txt
git push