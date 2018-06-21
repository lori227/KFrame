#!/bin/sh

deploypath=~/Survive
binpath=../../_bin/linux
respath=../../../Resource
settingpath=../../_bin/setting

#setting
cp -rf $settingpath $deploypath/Source/_bin/

#config
cp -rf $respath/config $deploypath/Resource/

#script
cp -rf $respath/script $deploypath/Resource/

#deploy
cp -f ./*.sh $deploypath/Source/_build/linux/

#plugin
cp -rf $binpath $deploypath/Source/_bin/


basepath=$(cd `dirname $0`; pwd)
cd $deploypath
git add -f *
git commit -m "deploy file update" *
git push

cd $basepath


