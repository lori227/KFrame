
basepath=$(cd `dirname $0`;pwd)

cd $basepath
git pull
sh make_develop.sh 0

version=`cat /tmp/projectversion`
echo $version

curl -d '{"command":"resource","value":"fighter_resource_'$version'.tar.gz","appname":"*","apptype":"*","appid":"*","zoneid":"0"}' http://180.169.238.146:9999/deploy
