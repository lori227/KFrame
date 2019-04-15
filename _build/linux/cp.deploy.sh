#!/bin/sh

respath=../../_bin
deploypath=/tmp/deploy

mkdir -p $deploypath
mkdir -p $deploypath/lib
mkdir -p $deploypath/bin
mkdir -p $deploypath/setting
mkdir -p $deploypath/startup

#shell
cp -a -f ./run.deploy.agent.sh $deploypath/
cp -a -f ./run.deploy.server.sh $deploypath/
cp -a -f ./kill.deploy.sh $deploypath/

#setting
cp -a -rf  $respath/setting/* $deploypath/setting/

#startup
cp -a -rf  $respath/startup/* $deploypath/startup/


#lib
cp -f $respath/lib/* $deploypath/lib/

#bin
binpath=$respath/bin/linux/release
cp -f $binpath/KFStartup $deploypath/bin/kfdeploy

cp -f $binpath/KFBus.so $deploypath/bin/
cp -f $binpath/KFIpAddress.so $deploypath/bin/
cp -f $binpath/KFTimer.so $deploypath/bin/
cp -f $binpath/KFHttpClient.so $deploypath/bin/
cp -f $binpath/KFHttpServer.so $deploypath/bin/
cp -f $binpath/KFTcpServer.so $deploypath/bin/
cp -f $binpath/KFTcpClient.so $deploypath/bin/
cp -f $binpath/KFMessage.so $deploypath/bin/
cp -f $binpath/KFConfig.so $deploypath/bin/
cp -f $binpath/KFDeployAgent.so $deploypath/bin/
cp -f $binpath/KFDeployServer.so $deploypath/bin/
cp -f $binpath/KFMySQL.so $deploypath/bin/
cp -f $binpath/KFSchedule.so $deploypath/bin/


cd /tmp
tar -zxvf deploy.tar.gz deploy/*
rm $deploypath -rf

mv deploy.tar.gz /data/