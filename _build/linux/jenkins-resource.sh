
basepath=$(cd `dirname $0`;pwd)

cd $basepath
git pull
sh make_develop.sh 0

version=`cat /tmp/projectversion`
echo $version

curl -d '{"command":"resource","value":"'$1'_resource_'$version'.tar.gz","appname":"*","apptype":"*","appid":"*","zoneid":"0"}' $2
