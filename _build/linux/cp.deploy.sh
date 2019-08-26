#!/bin/sh

respath=../../_bin
deploypath=/tmp/deploy

mkdir -p $deploypath
mkdir -p $deploypath/lib
mkdir -p $deploypath/bin
mkdir -p $deploypath/setting
mkdir -p $deploypath/startup
mkdir -p $deploypath/config

#shell
cp -a -f ./run.deploy.agent.sh $deploypath/
cp -a -f ./run.deploy.server.sh $deploypath/
cp -a -f ./kill.deploy.sh $deploypath/

#setting
cp -a -rf  $respath/setting/* $deploypath/setting/

#startup
cp -a -rf  $respath/startup/* $deploypath/startup/

#config
cp -a -rf  $respath/config/config.xml $deploypath/config/

#lib
cp -f $respath/lib/* $deploypath/lib/

filename=""
if [ $1 == "debug" ];then
	filename="d"
fi

#bin
binpath=$respath/bin/linux/$1
cp -f $binpath/KFStartup$filename $deploypath/bin/kfdeploy

cp -f $binpath/KFBus$filename.so $deploypath/bin/
cp -f $binpath/KFIpAddress$filename.so $deploypath/bin/
cp -f $binpath/KFTimer$filename.so $deploypath/bin/
cp -f $binpath/KFHttpClient$filename.so $deploypath/bin/
cp -f $binpath/KFHttpServer$filename.so $deploypath/bin/
cp -f $binpath/KFTcpServer$filename.so $deploypath/bin/
cp -f $binpath/KFTcpClient$filename.so $deploypath/bin/
cp -f $binpath/KFMessage$filename.so $deploypath/bin/
cp -f $binpath/KFConfig$filename.so $deploypath/bin/
cp -f $binpath/KFDeployAgent$filename.so $deploypath/bin/
cp -f $binpath/KFDeployServer$filename.so $deploypath/bin/
cp -f $binpath/KFMySQL$filename.so $deploypath/bin/
cp -f $binpath/KFSchedule$filename.so $deploypath/bin/


cd /tmp
tar -zcvf deploy.tar.gz deploy/*
rm $deploypath -rf

mkdir -p /data
mv deploy.tar.gz /data/
