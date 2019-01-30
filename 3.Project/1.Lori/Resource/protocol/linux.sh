#! /bin/sh 
cd proto
sh __cpp_class.sh
cd ../
\cp *.pb.* ../../Server/KFContrib/KFProtocol/
rm -f *.pb.*

# make proto version
function makeversion()
{
	oldmd5client=`cat .md5list | grep "$1" | cut -d ":" -f 2`
	newmd5client=`md5sum ./proto/$1 | awk '{print $1}'`
			
	declare -i version=`cat .md5list | grep "$1" | cut -d ":" -f 3`
	if [ $oldmd5client != $newmd5client ];then
		declare -i newversion=$version+1
		sed -i "s/$oldmd5client:$version/$newmd5client:$newversion/g" .md5list
		version=$newversion
	fi

	return $version
}


# client proto
makeversion KFClientMessage.proto
clientversion=$?
echo "client version=$clientversion"

makeversion KFDefineMessage.proto
battleversion=$?
echo "battle version=$battleversion"

# write in version file
echo "$clientversion.$battleversion.0.0" > version.txt

#svn 
svn ci -m "proto version" .md5list version.txt

#git
git commit -m "proto version" .md5list version.txt
git push


