#!/bin/sh

respath=../../_bin
deploypath=/data/deploy

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
cp -f $respath/bin/release/KFStartup $deploypath/bin/kfdeploy

cp -f $respath/bin/release/KFBus.so $deploypath/bin/
cp -f $respath/bin/release/KFIpAddress.so $deploypath/bin/
cp -f $respath/bin/release/KFTimer.so $deploypath/bin/
cp -f $respath/bin/release/KFHttpClient.so $deploypath/bin/
cp -f $respath/bin/release/KFHttpServer.so $deploypath/bin/
cp -f $respath/bin/release/KFTcpServer.so $deploypath/bin/
cp -f $respath/bin/release/KFTcpClient.so $deploypath/bin/
cp -f $respath/bin/release/KFMessage.so $deploypath/bin/
cp -f $respath/bin/release/KFConfig.so $deploypath/bin/
cp -f $respath/bin/release/KFDeployAgent.so $deploypath/bin/
cp -f $respath/bin/release/KFDeployServer.so $deploypath/bin/
cp -f $respath/bin/release/KFMySQL.so $deploypath/bin/
cp -f $respath/bin/release/KFSchedule.so $deploypath/bin/
